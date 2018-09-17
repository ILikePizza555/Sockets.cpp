//
// Created by avris on 9/17/2018.
//

#include <algorithm>
#include <Byte.h>

ByteString::ByteString() : _data(nullptr), size(0)
{
}

ByteString::ByteString(ByteBuffer buf) : _data(std::make_unique<byte[]>(buf.size())), size(buf.size())
{
    std::copy(buf.cbegin(), buf.cend(), _data.get());
}

ByteString::ByteString(std::initializer_list<byte> bytes) : _data(std::make_unique<byte[]>(bytes.size())), size(bytes.size())
{
    std::copy(bytes.begin(), bytes.end(), _data.get());
}

ByteString::ByteString(size_t size, std::unique_ptr<byte[]> data) : _data(std::move(data)), size(size)
{
}

byte *
ByteString::begin()
{
    return _data.get();
}

byte *
ByteString::end()
{
    return _data.get() + size;
}

const byte *
ByteString::cbegin() const
{
    return _data.get();
}

const byte*
ByteString::cend() const
{
    return _data.get() + size;
}

byte &
ByteString::operator[](const size_t n)
{
    if (n >= size) throw std::out_of_range("n is larger than size");

    return _data[n];
}

const byte &
ByteString::operator[](const size_t n) const
{
    if (n >= size) throw std::out_of_range("n is larger than size");

    return _data[n];
}
