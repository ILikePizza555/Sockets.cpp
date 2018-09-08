//
// Created by avris on 9/5/2018.
//

#pragma once

#include "Buffer.h"
#include "ByteString.h"
#include <memory>

/**
 * "Helper struct" that consists of a pointer to data and the size.
 *
 * This class exists solely to assist with the implementation of other classes.
 */

template<typename T>
class Buffer
{
private:
    std::unique_ptr<T[]> buffer_ptr = nullptr;
    size_t _capacity = 0;

public:
    explicit Buffer(size_t capacity);

    /**
     * @return An iterator to the beginning of the buffer.
     */
    T*
    begin();

    /**
     * @return An iterator to the end of the buffer.
     */
    T*
    end();

    /**
     * @return A constant iterator to the beginning of the buffer.
     */
     const T*
     cbegin() const;

     /**
      * @return A constant iterator to the end of the buffer.
      */
      const T*
      cend() const;

    /**
     * Writes data from the iterator to the buffer.
     *
     * If more data is provided than fits the buffer capacity, write() will return before the iterator reaches end.
     *
     * @tparam Iter
     * @return Number of items written
     */
    template<typename Iter> size_t
    write(Iter in, Iter end);

    /**
     * Reads the buffer data out to the iterator
     * @tparam Iter
     * @param out
     * @return Number of items read
     */
    template<typename Iter> size_t
    read(Iter out) const;

    /**
     * Resizes the Buffer. If new_capacity is smaller than current capacity, the data will be truncated.
     * @param new_capacity
     */
    void
    resize(size_t new_capacity);

    size_t
    capacity() const;

    std::unique_ptr<T[]>&
    get() const;

    /**
     * Converts the buffer to a ByteString. This operation clears the buffer.
     * @return A ByteString containing the buffer's data.
     */
    ByteString
    to_bytestring();

    T&
    operator[](size_t index);

    const T&
    operator[](size_t index) const;

    bool
    operator==(const Buffer<T>& other) const;
};