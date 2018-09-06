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
 * This class exists solely to assist with the implementation of other classes, and is not part of the API.
 */

template<typename T>
class Buffer
{
private:
    std::unique_ptr<T[]> buffer_ptr = nullptr;
    size_t _capacity = 0;

public:
    Buffer(size_t capacity);

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
     * Reads data from the iterator into the buffer.
     * @tparam Iter
     * @return Number of items read
     */
    template<typename Iter> size_t
    read(Iter in, Iter end);

    /**
     * Writes the buffer data out to the iterator
     * @tparam Iter
     * @param out
     * @return Number of items written
     */
    template<typename Iter> size_t
    write(Iter out);

    /**
     * Resizes the Buffer. If new_capacity is smaller than current capacity, the data will be truncated.
     * @param new_capacity
     */
    void
    resize(size_t new_capacity);

    size_t
    capacity();

    std::unique_ptr<T[]>&
    get();

    ByteString
    to_bytestring();

    T&
    operator[](size_t index);

    const T&
    operator[](size_t index) const;

    bool
    operator==(const Buffer& other) const;
};