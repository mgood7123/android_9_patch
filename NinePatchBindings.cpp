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

// jobject NinePatchPeeker::createNinePatchInsets(JNIEnv* env, float scale) const {
//     if (!mHasInsets) {
//         return nullptr;
//     }
//
//     return env->NewObject(gInsetStruct_class, gInsetStruct_constructorMethodID,
//             mOpticalInsets[0], mOpticalInsets[1],
//             mOpticalInsets[2], mOpticalInsets[3],
//             mOutlineInsets[0], mOutlineInsets[1],
//             mOutlineInsets[2], mOutlineInsets[3],
//             mOutlineRadius, mOutlineAlpha, scale);
// }
//
// void NinePatchPeeker::getPadding(JNIEnv* env, jobject outPadding) const {
//     if (mPatch) {
//         GraphicsJNI::set_jrect(env, outPadding,
//                 mPatch->paddingLeft, mPatch->paddingTop,
//                 mPatch->paddingRight, mPatch->paddingBottom);
//
//     } else {
//         GraphicsJNI::set_jrect(env, outPadding, -1, -1, -1, -1);
//     }
// }
