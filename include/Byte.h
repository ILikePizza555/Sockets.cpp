#pragma once

#include <memory>
#include <vector>

using byte = unsigned char;
using ByteBuffer = std::vector<byte>;

template<size_t size>
using ByteString = std::array<byte, size>;