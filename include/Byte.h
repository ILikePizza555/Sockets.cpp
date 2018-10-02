#pragma once

#include <ostream>
#include <memory>
#include <vector>

using byte = unsigned char;
using ByteBuffer = std::vector<byte>;

template<size_t size>
using ByteString = std::array<byte, size>;

std::ostream& operator<<(std::ostream& out, ByteBuffer b);

template<size_t size>
std::ostream& operator<<(std::ostream& out, ByteString<size> b)
{
    out << "ByteString size=" << size << ":";
    for(auto& c : b) out << c;
    return out;
}