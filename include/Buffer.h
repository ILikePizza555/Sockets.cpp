//
// Created by avris on 9/5/2018.
//

#pragma once

#include <memory>

/**
 * "Helper struct" that consists of a pointer to data and the size.
 *
 * This class exists solely to assist with the implementation of other classes, and is not part of the API.
 */

template<typename T>
struct Buffer
{
    std::unique_ptr<T> buffer_ptr = nullptr;
    size_t capacity = 0;

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
};