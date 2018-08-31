#include <ByteString.h>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

namespace sockets {
    ByteString::ByteString() : _data(nullptr), _size(0) {}
    ByteString::ByteString(std::initializer_list<byte> l) : _data(new byte[l.size()]), _size(l.size())
    {
        std::copy(l.begin(), l.end(), _data.get());
    }
    //Constructs a new ByteString by copying data
    ByteString::ByteString(data_ptr& data, size_t size) : _data(new byte[size]), _size(size) 
    {
        std::copy(data.get(), data.get() + size, _data.get());
    }
    //Constructs a new ByteString by moving data
    ByteString::ByteString(data_ptr&& data, size_t size) : _data(std::move(data)), _size(size)
    {
    }
    ByteString::ByteString(const ByteString& other) : _data(new byte[other._size]), _size(other._size)
    {
        std::copy(other.cbegin(), other.cend(), _data.get());
    }
    ByteString::ByteString(ByteString&& other) : _data(std::move(other._data)), _size(other._size) 
    {
        other._size = 0;
    }
    ByteString::~ByteString() 
    {
    }

    // Copy assignment
    ByteString& ByteString::operator=(const ByteString& other)
    {
        if(this != &other) 
        {
            // Reallocate memory if needed
            if (_size != other._size) 
            {
                _data.reset(new byte[other._size]);
                _size = other._size;
            }
            // Copy the data
            std::copy(other.cbegin(), other.cend(), _data.get());
        }
        return *this;
    }

    ByteString& ByteString::operator=(ByteString&& other)
    {
        if(this != &other) 
        {
            _data = std::move(other._data);
            
            _size = other._size;
            other._size = 0;
        }
        return *this;
    }

    ByteString::const_iterator ByteString::cbegin() const
    {
        return _data.get();
    }

    ByteString::const_iterator ByteString::cend() const
    {
        return _data.get() + _size;
    }

    ByteString ByteString::append(const ByteString& b) const
    {
        return this->append(b._data.get(), b._size);
    }

    ByteString ByteString::append(const byte* bytes, size_t bytes_length) const
    {
        size_t new_size = _size + bytes_length;
        data_ptr data(new byte[new_size]);

        std::copy(_data.get(), _data.get() + _size, data.get());
        std::copy(bytes, bytes + bytes_length, data.get() + _size);

        return ByteString(std::move(data), new_size);
    }

    ByteString ByteString::replace(const ByteString b, size_t begin_pos) const
    {
        return this->replace(b._data.get(), b._size, begin_pos);
    }

    ByteString ByteString::replace(const byte* bytes, size_t bytes_length, size_t begin_pos) const
    {
        if(begin_pos > _size) throw std::out_of_range("pos is larger than ByteString size");

        size_t new_size = _size + (bytes_length - _size + begin_pos );
        data_ptr data(new byte[new_size]);

        //Copy the data that remains the same
        std::copy(_data.get(), _data.get() + begin_pos, data.get());
        // Copy new data
        std::copy(bytes, bytes + bytes_length, data.get() + begin_pos);

        return ByteString(std::move(data), new_size);
    }

    ByteString ByteString::insert(const ByteString b, size_t pos) const
    {
        if(pos > _size) throw std::out_of_range("pos is larger than ByteString size");

        return this->insert(b._data.get(), b._size, pos);
    }

    ByteString ByteString::insert(const byte* bytes, size_t bytes_length, size_t pos) const
    {
        if(pos > _size) throw std::out_of_range("pos is larger than ByteString size");

        size_t new_size = _size + bytes_length;
        data_ptr data(new byte[new_size]);

        // Copy up to the insertion point
        std::copy(_data.get(), _data.get() + pos, data.get());
        // Copy new data
        std::copy(bytes, bytes + bytes_length, data.get() + pos);
        // Copy the rest of the data
        std::copy(_data.get() + pos, _data.get() + _size, data.get() + pos + bytes_length);

        return ByteString(std::move(data), new_size);
    }

    ByteString ByteString::sub(size_t begin_pos) const
    {
        return sub(begin_pos, _size);
    }

    ByteString ByteString::sub(size_t begin_pos, size_t end_pos) const
    {
        if (begin_pos > end_pos)
            throw std::out_of_range("Beginning index is larger than end index");
        if (end_pos > _size)
            throw std::out_of_range("End index is larger than ByteString size");

        size_t new_size = end_pos - begin_pos;
        data_ptr data(new byte[new_size]);

        std::copy(_data.get() + begin_pos, _data.get() + end_pos, data.get());

        return ByteString(std::move(data), new_size);
    }

    byte ByteString::at(size_t pos) const
    {
        if (pos >= _size) throw std::out_of_range("Index 'pos' out of range.");
        return *(_data.get() + pos);
    }

    size_t ByteString::size() const
    {
        return _size;
    }

    bool ByteString::operator==(const ByteString& o) const
    {
        if(_size != o._size) return false;

        auto mine = cbegin();
        auto mine_end = cend();

        auto other = o.cbegin();
        auto other_end = o.cend();

        while(mine != mine_end && other != other_end)
        {
            if(*mine != *other) return false;
            ++mine;
            ++other;
        }

        return true;
    }

    bool ByteString::operator!=(const ByteString& o) const
    {
        return !(*this == o);
    }

    std::ostream& operator<<(std::ostream& out, const ByteString& b)
    {
        for(auto i = b.cbegin(); i != b.cend(); ++i)
        {
            out << std::setw(2) << std::setfill('0') << std::hex << int(*i) << " ";
        }
        return out;
    }
}
