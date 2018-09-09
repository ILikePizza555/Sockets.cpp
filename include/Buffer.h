//
// Created by avris on 9/5/2018.
//

#pragma once

#include <memory>

/**
 * Consists of a pointer to data and the size.
 *
 * This class exists solely to assist with the implementation of other classes.
 */

template<typename T>
class Buffer
{
protected:
    std::unique_ptr<T[]> buffer_ptr = nullptr;
    size_t _capacity = 0;

public:
    explicit Buffer(size_t capacity) : buffer_ptr(std::make_unique<T[]>(capacity)), _capacity(capacity) {}

    /**
     * @return An iterator to the beginning of the buffer.
     */
    T*
    begin()
    {
        return buffer_ptr.get();
    }

    /**
     * @return An iterator to the end of the buffer.
     */
    T*
    end()
    {
        return buffer_ptr.get() + _capacity;
    }

    /**
     * @return A constant iterator to the beginning of the buffer.
     */
     const T*
     cbegin() const
    {
         return buffer_ptr.get();
    }

     /**
      * @return A constant iterator to the end of the buffer.
      */
      const T*
      cend() const
     {
          return buffer_ptr.get() + _capacity;
     }

    /**
     * Writes data from the iterator to the buffer.
     *
     * If more data is provided than fits the buffer capacity, write() will return before the iterator reaches end.
     *
     * @tparam Iter
     * @return Number of items written
     */
    template<typename Iter> size_t
    write(Iter in, Iter end)
    {
        size_t i;
        for(i = 0; i < _capacity; ++i)
        {
            if (in == end) return i;

            buffer_ptr[i] = *in;
            ++in;
        }

        return i;
    }

    /**
     * Reads the buffer data out to the iterator
     * @tparam Iter
     * @param out
     * @return Number of items read
     */
    template<typename Iter> size_t
    read(Iter out) const
    {
        size_t i;
        for(i = 0; i < _capacity; ++i)
        {
            *out = buffer_ptr[i];
            ++out;
        }
        return i;
    }

    /**
     * Resizes the Buffer. If new_capacity is smaller than current capacity, the data will be truncated.
     * @param new_capacity
     */
    void
    resize(size_t new_capacity)
    {
        std::unique_ptr<T[]> new_buffer = std::make_unique<T[]>(new_capacity);

        if(buffer_ptr != nullptr)
            std::copy(buffer_ptr.get(), buffer_ptr.get() + std::min(_capacity, new_capacity), new_buffer.get());

        buffer_ptr = new_buffer;
        _capacity = new_capacity;
    }


    size_t
    capacity() const
    {
        return _capacity;
    }

    std::unique_ptr<T[]>&
    get() const
    {
        return buffer_ptr;
    }

    T&
    operator[](size_t index)
    {
        if(index >= _capacity) throw std::out_of_range("index out of range");
        return buffer_ptr[index];
    }

    const T&
    operator[](size_t index) const
    {
        if(index >= _capacity) throw std::out_of_range("index out of range");
        return buffer_ptr[index];
    }

    bool
    operator==(const Buffer<T>& other) const
    {
        if(other._capacity != _capacity) return false;

        for(size_t i = 0; i < _capacity; ++i)
        {
            if(other[i] != operator[](i)) return false;
        }

        return true;
    }
};