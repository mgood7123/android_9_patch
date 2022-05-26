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

// TODO: create bindings

#if !defined(CSHARP_BINDING_API)
#if defined(_MSC_VER)
#define CSHARP_BINDING_API extern "C" __declspec(dllexport)
#else
#define CSHARP_BINDING_API extern "C" __attribute__((visibility("default")))
#endif
#endif

CSHARP_BINDING_API int8_t SkNinePatchGlue_isNinePatchChunk(int8_t* array, int32_t length);

CSHARP_BINDING_API int8_t* SkNinePatchGlue_validateNinePatchChunk(int8_t* array, int32_t length);

CSHARP_BINDING_API void SkNinePatchGlue_finalize(int8_t* patch);
