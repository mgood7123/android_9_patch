/*
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#undef LOG_TAG
#define LOG_TAG "9patch"

#include "NinePatchBindings.h"
#include <SkScalar.h>

//#include "NinePatchPeeker.h"
//#include "NinePatchUtils.h"

using namespace android;

/**
 * IMPORTANT NOTE: 9patch chunks can be manipuated either as an array of bytes
 * or as a Res_png_9patch instance. It is important to note that the size of the
 * array required to hold a 9patch chunk is greater than sizeof(Res_png_9patch).
 * The code below manipulates chunks as Res_png_9patch* types to draw and as
 * int8_t* to allocate and free the backing storage.
 */
CSHARP_BINDING_API int8_t SkNinePatchGlue_isNinePatchChunk(int8_t * array, int32_t length) {
    if (nullptr == array) {
        return false;
    }
    if (length < (int32_t)sizeof(Res_png_9patch)) {
        return false;
    }
    const Res_png_9patch* chunk = reinterpret_cast<const Res_png_9patch*>(array);
    int8_t wasDeserialized = chunk->wasDeserialized;
    return (wasDeserialized != -1) ? 1 : 0;
}

CSHARP_BINDING_API int8_t * SkNinePatchGlue_validateNinePatchChunk(int8_t * array, int32_t length) {
    size_t chunkSize = length;
    if (chunkSize < (int) (sizeof(Res_png_9patch))) {
        return nullptr;
    }

    int8_t* storage = new int8_t[chunkSize];
    memcpy(storage, array, chunkSize*sizeof(int8_t));
    // This call copies the content of the jbyteArray
    //env->GetByteArrayRegion(obj, 0, chunkSize, reinterpret_cast<jbyte*>(storage));
    // Deserialize in place, return the array we just allocated
    return reinterpret_cast<int8_t*>(Res_png_9patch::deserialize(storage));
}

CSHARP_BINDING_API void SkNinePatchGlue_finalize(int8_t * patch) {
    delete[] patch;
}

CSHARP_BINDING_API bool SkNinePatchGlue_ReadChunk(
    // ReadChunk
    const char* tag, const void* data, size_t length,
    // NPatch
    void ** mPatch, size_t * mPatchSize, bool * mHasInsets,
    int32_t ** mOpticalInsets, int32_t ** mOutlineInsets,
    float * mOutlineRadius, uint8_t * mOutlineAlpha
) {
    if (!strcmp("npTc", tag) && length >= sizeof(Res_png_9patch)) {
        Res_png_9patch* patch = (Res_png_9patch*)data;
        size_t patchSize = patch->serializedSize();
        if (length != patchSize) {
            return false;
        }
        // You have to copy the data because it is owned by the png reader
        Res_png_9patch* patchNew = (Res_png_9patch*)malloc(patchSize);
        memcpy(patchNew, patch, patchSize);
        Res_png_9patch::deserialize(patchNew);
        patchNew->fileToDevice();
        free(*mPatch);
        *mPatch = patchNew;
        *mPatchSize = patchSize;
    }
    else if (!strcmp("npLb", tag) && length == sizeof(int32_t) * 4) {
        *mHasInsets = true;
        memcpy(mOpticalInsets, data, sizeof(int32_t) * 4);
    }
    else if (!strcmp("npOl", tag) && length == 24) { // 4 int32_ts, 1 float, 1 int32_t sized byte
        *mHasInsets = true;
        memcpy(mOutlineInsets, data, sizeof(int32_t) * 4);
        *mOutlineRadius = ((const float*)data)[4];
        *mOutlineAlpha = ((const int32_t*)data)[5] & 0xff;
    }
    return true;    // keep on decoding
}

// static jlong getTransparentRegion(JNIEnv* env, jobject, jlong bitmapPtr,
//         jlong chunkHandle, jobject dstRect) {
//     Res_png_9patch* chunk = reinterpret_cast<Res_png_9patch*>(chunkHandle);
//     SkASSERT(chunk);
//
//     SkBitmap bitmap;
//     bitmap::toBitmap(bitmapPtr).getSkBitmap(&bitmap);
//     SkRect dst;
//     GraphicsJNI::jrect_to_rect(env, dstRect, &dst);
//
//     SkCanvas::Lattice lattice;
//     SkIRect src = SkIRect::MakeWH(bitmap.width(), bitmap.height());
//     lattice.fBounds = &src;
//     NinePatchUtils::SetLatticeDivs(&lattice, *chunk, bitmap.width(), bitmap.height());
//     lattice.fRectTypes = nullptr;
//     lattice.fColors = nullptr;
//
//     SkRegion* region = nullptr;
//     if (SkLatticeIter::Valid(bitmap.width(), bitmap.height(), lattice)) {
//         SkLatticeIter iter(lattice, dst);
//         if (iter.numRectsToDraw() == chunk->numColors) {
//             SkRect dummy;
//             SkRect iterDst;
//             int index = 0;
//             while (iter.next(&dummy, &iterDst)) {
//                 if (0 == chunk->getColors()[index++] && !iterDst.isEmpty()) {
//                     if (!region) {
//                         region = new SkRegion();
//                     }
//
//                     region->op(iterDst.round(), SkRegion::kUnion_Op);
//                 }
//             }
//         }
//     }
//
//     return reinterpret_cast<jlong>(region);
// }


CSHARP_BINDING_API void SkNinePatchGlue_getPadding(
    void** mPatch, int32_t** outPadding
) {
    Res_png_9patch* patch = (Res_png_9patch*)(*mPatch);
    int32_t* padding = *outPadding;
    if (patch) {
        padding[0] = patch->paddingLeft;
        padding[1] = patch->paddingTop;
        padding[2] = patch->paddingRight;
        padding[3] = patch->paddingBottom;
     } else {
        padding[0] = -1;
        padding[1] = -1;
        padding[2] = -1;
        padding[3] = -1;
    }
}

static void scaleDivRange(int32_t* divs, int count, float scale, int maxValue) {
    for (int i = 0; i < count; i++) {
        divs[i] = int32_t(divs[i] * scale + 0.5f);
        if (i > 0 && divs[i] == divs[i - 1]) {
            divs[i]++; // avoid collisions
        }
    }

    if (CC_UNLIKELY(divs[count - 1] > maxValue)) {
        // if the collision avoidance above put some divs outside the bounds of the bitmap,
        // slide outer stretchable divs inward to stay within bounds
        int highestAvailable = maxValue;
        for (int i = count - 1; i >= 0; i--) {
            divs[i] = highestAvailable;
            if (i > 0 && divs[i] <= divs[i - 1]) {
                // keep shifting
                highestAvailable = divs[i] - 1;
            }
            else {
                break;
            }
        }
    }
}

CSHARP_BINDING_API void SkNinePatchGlue_scale(
    // NPatch
    void** mPatch,
    // scale
    float scaleX, float scaleY, int scaledWidth, int scaledHeight
) {
    Res_png_9patch* patch = (Res_png_9patch*)(*mPatch);
    if (!(patch)) {
        return;
    }

    // The max value for the divRange is one pixel less than the actual max to ensure that the size
    // of the last div is not zero. A div of size 0 is considered invalid input and will not render.
    if (!SkScalarNearlyEqual(scaleX, 1.0f)) {
        patch->paddingLeft = int(patch->paddingLeft * scaleX + 0.5f);
        patch->paddingRight = int(patch->paddingRight * scaleX + 0.5f);
        scaleDivRange(patch->getXDivs(), patch->numXDivs, scaleX, scaledWidth - 1);
    }

    if (!SkScalarNearlyEqual(scaleY, 1.0f)) {
        patch->paddingTop = int(patch->paddingTop * scaleY + 0.5f);
        patch->paddingBottom = int(patch->paddingBottom * scaleY + 0.5f);
        scaleDivRange(patch->getYDivs(), patch->numYDivs, scaleY, scaledHeight - 1);
    }
}
