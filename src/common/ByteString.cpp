#include <include/ByteString.h>
#include <algorithm>

/**
 * Allocates new space for a copy of the data, copies it, and then returns it as a const
 */
const sockets::byte* allocate_and_copy(const sockets::byte* data, size_t size)
{
    sockets::byte* rv = new sockets::byte[size];
    std::copy(data, data + size, rv);
    return static_cast<const sockets::byte*>(rv);
}
namespace sockets {
    ByteString::ByteString() : _data(nullptr), _size(0) {}
    ByteString::ByteString(byte* data, size_t size) : _data(data), _size(size) {}
    ByteString::ByteString(ByteString& other) : _data(allocate_and_copy(other._data, other._size)), _size(other.size) {}
    ByteString::~ByteString() 
    {
        delete [] _data;
        _data = nullptr;
    }
}