#include <include/ByteString.h>
#include <algorithm>


namespace sockets {
    ByteString::ByteString() : _data(nullptr), _size(0) {}
    ByteString::ByteString(byte* data, size_t size) : _data(data), _size(size) {}
    ByteString::ByteString(const ByteString& other) : _data(new byte[other.size]), _size(other.size)
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

    ByteString& ByteString::operator=(const ByteString& other)
    {
        if(this != &other) 
        {
            // Reallocate memory if needed
            if (_size != other._size) 
            {
                delete [] _data;
                _data = new byte[other.size];
                _size = other.size;
            }
            // Copy the data
            std::copy(other.cbegin(), other.cend(), _data);
        }
        return *this;
    }

    ByteString& ByteString::operator=(ByteString&& other)
    {
        if(this != &other) 
        {
            // Free data if needed
            if (_size > 0) 
            {
                delete [] _data;
                _data = nullptr;
                _size = 0;
            }

            // Copy objects here
            _data = other._data;
            _size = other._size;

            // Free other's resources
            delete [] other._data;
            other._size = 0;
        }
    }

    ByteString::const_iterator ByteString::cbegin() const
    {
        return _data;
    }

    ByteString::const_iterator ByteString::cend() const
    {
        return _data + _size;
    }

    ByteString ByteString::append(const ByteString& b) const
    {
        return this->append(b._data, b._size);
    }

    ByteString ByteString::append(const byte* bytes, size_t bytes_length) const
    {
        size_t new_size = _size + bytes_length;
        byte* data = new byte[new_size];

        std::copy(_data, _data + _size, data);
        std::copy(bytes, bytes + bytes_length, data + _size);

        return ByteString(data, new_size);
    }
}