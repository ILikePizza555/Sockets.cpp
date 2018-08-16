#pragma once

#include <cstddef>
#include "Byte.h"

namespace sockets {
    /**
     * A ByteString is an immutable sequence of binary data.
     */
    class ByteString {
    private:
        const byte* _data;
        const size_t _size;
    public:
        ByteString();
        ByteString(byte* data, size_t size);
        ByteString(ByteString& other);
        ByteString(ByteString&& other);
        ~ByteString();

        ByteString& operator=(const ByteString& other) const;
        ByteString& operator=(const ByteString&& other) const;

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
         */
        ByteString replace(const ByteString b, size_t begin_pos) const;
        ByteString replace(const byte* bytes, size_t bytes_length, size_t begin_pos) const;

        /**
         * Constructs a new ByteString by inserting the bytes at the given position.
         */
        ByteString insert(const ByteString b, size_t pos) const;
        ByteString insert(const byte* bytes, size_t bytes_length, size_t pos) const;

        /**
         * Constructs a new ByteString by copying the range of bytes between the given starting position and the end.
         * Throws std::out_of_range if the index is less than 0 or larger than the ByteString.
         */
        ByteString sub(size_t begin_pos) const;
        /**
         * Constructs a new ByteString by copying the range of bytes between the given starting position and the given end position.
         * 
         * Throws std::out_of_range if begin_pos is less than 0 or greater than end_pos. 
         * Throws std::out_of_range if end_pos is less than 0 or greater than the size of ByteString.
         */
        ByteString sub(size_t begin_pos, size_t end_pos) const;

        /**
         * Returns the byte at the specified position
         */
        byte at(size_t pos) const;

        /**
         * Returns the size of the ByteString
         */
        size_t size() const;
    };
}