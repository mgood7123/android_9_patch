/*
 * Copyright (C) 2020 The Android Open Source Project
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

#include "FileMap.h"

#include "map_ptr.h"

namespace android::incfs {
IncFsFileMap::IncFsFileMap() noexcept = default;
IncFsFileMap::IncFsFileMap(IncFsFileMap&&) noexcept = default;
IncFsFileMap& IncFsFileMap::operator =(IncFsFileMap&&) noexcept = default;
IncFsFileMap::~IncFsFileMap() noexcept = default;

const void* IncFsFileMap::unsafe_data() const {
    return map_->getDataPtr();
}

size_t IncFsFileMap::length() const {
    return map_->getDataLength();
}

off64_t IncFsFileMap::offset() const {
    return map_->getDataOffset();
}

const char* IncFsFileMap::file_name() const {
    return map_->getFileName();
}

bool IncFsFileMap::Create(int fd, off64_t offset, size_t length, const char* file_name) {
    return Create(fd, offset, length, file_name, true /* verify */);
}

bool IncFsFileMap::Create(int fd, off64_t offset, size_t length, const char* file_name,
                          bool verify) {
    return CreateForceVerification(fd, offset, length, file_name, verify);
}

bool IncFsFileMap::CreateForceVerification(int fd, off64_t offset, size_t length,
                                           const char* file_name, bool /* verify */) {
    map_ = std::make_unique<android::FileMap>();
    return map_->create(file_name, fd, offset, length, true /* readOnly */);
}

bool IncFsFileMap::Verify(const uint8_t* const& /* data_start */,
                          const uint8_t* const& /* data_end */,
                          const uint8_t** /* prev_verified_block */) const {
    return true;
}
} // namespace android::incfs
