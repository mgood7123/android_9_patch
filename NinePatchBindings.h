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

#include "9patch.h"

#if !defined(CSHARP_BINDING_API)
#if defined(_MSC_VER)
#define CSHARP_BINDING_API extern "C" __declspec(dllexport)
#else
#define CSHARP_BINDING_API extern "C" __attribute__((visibility("default")))
#endif
#endif

CSHARP_BINDING_API bool SkNinePatchGlue_isNinePatchChunk(int8_t* array, int32_t length);

CSHARP_BINDING_API int8_t* SkNinePatchGlue_validateNinePatchChunk(int8_t* array, int32_t length);

CSHARP_BINDING_API void SkNinePatchGlue_finalize(int8_t* patch);

CSHARP_BINDING_API bool SkNinePatchGlue_ReadChunk(
    // ReadChunk
    const char* tag, const void* data, size_t length,
    // NPatch
    void** mPatch, size_t* mPatchSize, bool* mHasInsets,
    int32_t** mOpticalInsets, int32_t** mOutlineInsets,
    float* mOutlineRadius, uint8_t* mOutlineAlpha
);

CSHARP_BINDING_API void SkNinePatchGlue_delete(
    void* mPatch
);

CSHARP_BINDING_API void SkNinePatchGlue_getPadding(
    void* mPatch, int32_t** outPadding
);

CSHARP_BINDING_API void SkNinePatchGlue_getNumXDivs(
    void* mPatch, uint8_t* out
);

CSHARP_BINDING_API void SkNinePatchGlue_getNumYDivs(
    void* mPatch, uint8_t* out
);

CSHARP_BINDING_API void SkNinePatchGlue_getNumColors(
    void* mPatch, uint8_t* out
);

CSHARP_BINDING_API void SkNinePatchGlue_getXDivs(
    void* mPatch, int32_t** out
);

CSHARP_BINDING_API void SkNinePatchGlue_getYDivs(
    void* mPatch, int32_t** out
);

CSHARP_BINDING_API void SkNinePatchGlue_getColors(
    void* mPatch, uint32_t** out
);

CSHARP_BINDING_API void SkNinePatchGlue_scale(
    void* mPatch,
    float scaleX, float scaleY, int scaledWidth, int scaledHeight
);

CSHARP_BINDING_API size_t SkNinePatchGlue_serializedSize(
    void* mPatch
);

CSHARP_BINDING_API void c_memcpy(
    void* dst, void* src, size_t length
);

CSHARP_BINDING_API int32_t c_memcmp(
    void* buf1, void* buf2, size_t length
);

CSHARP_BINDING_API void* c_memset(
    void* buf, int value, size_t length
);
