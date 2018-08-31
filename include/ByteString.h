#pragma once

#include <cstddef>
#include <memory>
#include <initializer_list>
#include "Byte.h"

namespace sockets {
    using data_ptr = std::unique_ptr<byte[]>;

    /**
     * A ByteString is an immutable sequence of binary data.
     * 
     */
    class ByteString {
    private:
        data_ptr _data;
        size_t _size;
    public:
        ByteString();
        ByteString(std::initializer_list<byte> l);
        /**
         * Constructs a new ByteString by copying the given data
         */
        ByteString(data_ptr& data, size_t size);
        /**
         * Constructs a new ByteString by taking ownership of the data pointer
         */
        ByteString(data_ptr&& data, size_t size);
        ByteString(const ByteString& other);
        ByteString(ByteString&& other);
        ~ByteString();

        ByteString& operator=(const ByteString& other);
        ByteString& operator=(ByteString&& other);

        using const_iterator = const byte *;
        /**
         * Returns a constant iterator to the begnning of the ByteString
         */
        const_iterator cbegin() const;

        /**
         * Returns a const iterator to the end of the ByteString
         */
        const_iterator cend() const;

        /**
         * Constructs a new ByteString by appending bytes to the current ByteString.
         */
        ByteString append(const ByteString& b) const;
        ByteString append(const byte* bytes, size_t bytes_length) const;

        /**
         * Constructs a new ByteString by replacing the bytes beginning at begin_pos with the given data.
         * Acts like append when begin_pos is equivalent to the size.
         * Throws std::out_of_range if begin_pos is larger than size.
         */
        ByteString replace(const ByteString b, size_t begin_pos) const;
        ByteString replace(const byte* bytes, size_t bytes_length, size_t begin_pos) const;

        /**
         * Constructs a new ByteString by inserting the bytes before the given position.
         * Throws std::out_of_range is pos is larger than size.
         */
        ByteString insert(const ByteString b, size_t pos) const;
        ByteString insert(const byte* bytes, size_t bytes_length, size_t pos) const;

        /**
         * Constructs a new ByteString by copying the range of bytes between the given starting position and the end.
         * Throws std::out_of_range if the index is larger than the ByteString.
         */
        ByteString sub(size_t begin_pos) const;
        /**
         * Constructs a new ByteString by copying the range of bytes [begin_pos, end_pos)
         * 
         * Throws std::out_of_range if begin_pos is greater than end_pos, or if end_pos is greater than size.
         */
        ByteString sub(size_t begin_pos, size_t end_pos) const;

        /**
         * Returns the byte at the specified position.
         * 
         * Throws std::out_of_range if pos is larger than size.
         */
        byte at(size_t pos) const;

        /**
         * Returns the size of the ByteString
         */
        size_t size() const;

        bool operator==(const ByteString& o) const;
        bool operator!=(const ByteString& o) const;

        friend std::ostream& operator<<(std::ostream& out, const ByteString& b);
    };
}
