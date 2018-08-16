#include <include/ByteString.h>
#include <algorithm>


namespace sockets {
    ByteString::ByteString() : _data(nullptr), _size(0) {}
    ByteString::ByteString(byte* data, size_t size) : _data(data), _size(size) {}
    ByteString::ByteString(ByteString& other) : _data(new byte[other.size]), _size(other.size)
    {
        std::copy(other.cbegin(), other.cend(), _data);
    }
    ByteString::ByteString(ByteString&& other) : _data(std::move(other._data)), _size(std::move(other._size)) {}
    ByteString::~ByteString() 
    {
        delete [] _data;
        _data = nullptr;
        _size = 0;
    }

}