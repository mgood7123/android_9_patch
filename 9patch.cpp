/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "9patch.h"

#include <ctype.h>
#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <vector>

#ifndef INT32_MAX
#define INT32_MAX ((int32_t)(2147483647))
#endif

namespace android {

#if defined(_WIN32)
    #undef  nhtol
    #undef  htonl
    #define ntohl(x)    ( ((x) << 24) | (((x) >> 24) & 255) | (((x) << 8) & 0xff0000) | (((x) >> 8) & 0xff00) )
    #define htonl(x)    ntohl(x)
    #define ntohs(x)    ( (((x) << 8) & 0xff00) | (((x) >> 8) & 255) )
    #define htons(x)    ntohs(x)
#endif

// TODO: This code uses 0xFFFFFFFF converted to bag_set* as a sentinel value. This is bad practice.

// Standard C isspace() is only required to look at the low byte of its input, so
// produces incorrect results for UTF-16 characters.  For safety's sake, assume that
// any high-byte UTF-16 code point is not whitespace.
inline int isspace16(char16_t c) {
    return (c < 0x0080 && isspace(c));
}

template<typename T>
inline static T max(T a, T b) {
    return a > b ? a : b;
}

// range checked; guaranteed to NUL-terminate within the stated number of available slots
// NOTE: if this truncates the dst string due to running out of space, no attempt is
// made to avoid splitting surrogate pairs.
static void strcpy16_dtoh(char16_t* dst, const uint16_t* src, size_t avail)
{
    char16_t* last = dst + avail - 1;
    while (*src && (dst < last)) {
        char16_t s = dtohs(static_cast<char16_t>(*src));
        *dst++ = s;
        src++;
    }
    *dst = 0;
}

static status_t validate_chunk(const incfs::map_ptr<ResChunk_header>& chunk,
                               size_t minSize,
                               const incfs::map_ptr<uint8_t> dataEnd,
                               const char* name)
{
    if (!chunk) {
      return BAD_TYPE;
    }

    const uint16_t headerSize = dtohs(chunk->headerSize);
    const uint32_t size = dtohl(chunk->size);

    if (headerSize >= minSize) {
        if (headerSize <= size) {
            if (((headerSize|size)&0x3) == 0) {
                if ((size_t)size <= (size_t)(dataEnd-chunk.convert<uint8_t>())) {
                    return NO_ERROR;
                }
                printf("%s data size 0x%x extends beyond resource end %p.\n",
                     name, size, (void*)(dataEnd-chunk.convert<uint8_t>()));
                return BAD_TYPE;
            }
            printf("%s size 0x%x or headerSize 0x%x is not on an integer boundary.\n",
                 name, (int)size, (int)headerSize);
            return BAD_TYPE;
        }
        printf("%s size 0x%x is smaller than header size 0x%x.\n",
             name, size, headerSize);
        return BAD_TYPE;
    }
    printf("%s header size 0x%04x is too small.\n",
         name, headerSize);
    return BAD_TYPE;
}

static void fill9patchOffsets(Res_png_9patch* patch) {
    patch->xDivsOffset = sizeof(Res_png_9patch);
    patch->yDivsOffset = patch->xDivsOffset + (patch->numXDivs * sizeof(int32_t));
    patch->colorsOffset = patch->yDivsOffset + (patch->numYDivs * sizeof(int32_t));
}

void Res_png_9patch::deviceToFile()
{
    int32_t* xDivs = getXDivs();
    for (int i = 0; i < numXDivs; i++) {
        xDivs[i] = htonl(xDivs[i]);
    }
    int32_t* yDivs = getYDivs();
    for (int i = 0; i < numYDivs; i++) {
        yDivs[i] = htonl(yDivs[i]);
    }
    paddingLeft = htonl(paddingLeft);
    paddingRight = htonl(paddingRight);
    paddingTop = htonl(paddingTop);
    paddingBottom = htonl(paddingBottom);
    uint32_t* colors = getColors();
    for (int i=0; i<numColors; i++) {
        colors[i] = htonl(colors[i]);
    }
}

void Res_png_9patch::fileToDevice()
{
    int32_t* xDivs = getXDivs();
    for (int i = 0; i < numXDivs; i++) {
        xDivs[i] = ntohl(xDivs[i]);
    }
    int32_t* yDivs = getYDivs();
    for (int i = 0; i < numYDivs; i++) {
        yDivs[i] = ntohl(yDivs[i]);
    }
    paddingLeft = ntohl(paddingLeft);
    paddingRight = ntohl(paddingRight);
    paddingTop = ntohl(paddingTop);
    paddingBottom = ntohl(paddingBottom);
    uint32_t* colors = getColors();
    for (int i=0; i<numColors; i++) {
        colors[i] = ntohl(colors[i]);
    }
}

size_t Res_png_9patch::serializedSize() const
{
    // The size of this struct is 32 bytes on the 32-bit target system
    // 4 * int8_t
    // 4 * int32_t
    // 3 * uint32_t
    return 32
            + numXDivs * sizeof(int32_t)
            + numYDivs * sizeof(int32_t)
            + numColors * sizeof(uint32_t);
}

void* Res_png_9patch::serialize(const Res_png_9patch& patch, const int32_t* xDivs,
                                const int32_t* yDivs, const uint32_t* colors)
{
    // Use calloc since we're going to leave a few holes in the data
    // and want this to run cleanly under valgrind
    void* newData = calloc(1, patch.serializedSize());
    serialize(patch, xDivs, yDivs, colors, newData);
    return newData;
}

void Res_png_9patch::serialize(const Res_png_9patch& patch, const int32_t* xDivs,
                               const int32_t* yDivs, const uint32_t* colors, void* outData)
{
    uint8_t* data = (uint8_t*) outData;
    memcpy(data, &patch.wasDeserialized, 4);     // copy  wasDeserialized, numXDivs, numYDivs, numColors
    memcpy(data + 12, &patch.paddingLeft, 16);   // copy paddingXXXX
    data += 32;

    memcpy(data, xDivs, patch.numXDivs * sizeof(int32_t));
    data +=  patch.numXDivs * sizeof(int32_t);
    memcpy(data, yDivs, patch.numYDivs * sizeof(int32_t));
    data +=  patch.numYDivs * sizeof(int32_t);
    memcpy(data, colors, patch.numColors * sizeof(uint32_t));

    fill9patchOffsets(reinterpret_cast<Res_png_9patch*>(outData));
}

Res_png_9patch* Res_png_9patch::deserialize(void* inData)
{

    Res_png_9patch* patch = reinterpret_cast<Res_png_9patch*>(inData);
    patch->wasDeserialized = true;
    fill9patchOffsets(patch);

    return patch;
}
}
