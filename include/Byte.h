#pragma once

#include <memory>
#include <vector>

using byte = unsigned char;
using ByteBuffer = std::vector<byte>;

/**
 * Simple class consisting of a pointer to an array of bytes and it's length.
 *
 * This class is intended for use in moving around small amounts of data, where using ByteBuffer(std::vector<byte>)
 * doesn't make sense.
 */
class ByteString
{
private:
    std::unique_ptr<byte[]> _data;
public:
    const size_t size;

    /**
     * Constructs an empty ByteString
     */
    ByteString();

    /**
     * Constructs a ByteString by copying data from a buffer.
     * @param buffer
     */
    explicit ByteString(ByteBuffer buffer);

    /**
     * Constructs a ByteString from a list of bytes
     * @param bytes
     */
    ByteString(std::initializer_list<byte> bytes);

    /**
     * Constructs a ByteString by taking ownership of a pointer
     * @param size
     * @param data
     */
    ByteString(size_t size, std::unique_ptr<byte[]> data);

    byte* begin();
    byte* end();
    const byte* cbegin() const;
    const byte* cend() const;

    byte& operator[](size_t n);
    const byte& operator[](size_t n) const;
};