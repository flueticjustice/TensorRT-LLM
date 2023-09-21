/*
 * Copyright (c) 2022-2023, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tensorrt_llm/runtime/memoryCounters.h"

#include "tensorrt_llm/common/stringUtils.h"

#include <array>
#include <cmath>

namespace tc = tensorrt_llm::common;

namespace
{

auto constexpr kByteUnits = std::array{"B", "KB", "MB", "GB", "TB", "PB", "EB"};

std::string doubleBytesToString(double bytes, int precision)
{
    int unitIdx{0};

    while (std::abs(bytes) >= 1024.0 && unitIdx < kByteUnits.size() - 1)
    {
        bytes /= 1024.0;
        ++unitIdx;
    }
    auto const format = "%." + std::to_string(precision) + "f %s";
    return tc::fmtstr(format.c_str(), bytes, kByteUnits[unitIdx]);
}

} // namespace

namespace tensorrt_llm::runtime
{
thread_local MemoryCounters MemoryCounters::mInstance;

std::string MemoryCounters::bytesToString(SizeType bytes, int precision)
{
    return doubleBytesToString(static_cast<double>(bytes), precision);
}

std::string MemoryCounters::bytesToString(MemoryCounters::DiffType bytes, int precision)
{
    return doubleBytesToString(static_cast<double>(bytes), precision);
}

void MemoryCounters::allocate(MemoryType memoryType, MemoryCounters::SizeType size)
{
    switch (memoryType)
    {
    case MemoryType::kGPU: allocate<MemoryType::kGPU>(size); break;
    case MemoryType::kCPU: allocate<MemoryType::kCPU>(size); break;
    case MemoryType::kPINNED: allocate<MemoryType::kPINNED>(size); break;
    default: TLLM_THROW("Unknown memory type");
    }
}

void MemoryCounters::deallocate(MemoryType memoryType, MemoryCounters::SizeType size)
{
    auto const sizeDiff = -static_cast<DiffType>(size);
    switch (memoryType)
    {
    case MemoryType::kGPU: deallocate<MemoryType::kGPU>(size); break;
    case MemoryType::kCPU: deallocate<MemoryType::kCPU>(size); break;
    case MemoryType::kPINNED: deallocate<MemoryType::kPINNED>(size); break;
    default: TLLM_THROW("Unknown memory type");
    }
}
} // namespace tensorrt_llm::runtime