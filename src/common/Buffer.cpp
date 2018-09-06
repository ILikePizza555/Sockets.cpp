//
// Created by avris on 9/5/2018.
//

#include <Buffer.h>

template<typename T>
Buffer<T>::Buffer(size_t capacity) : buffer_ptr(std::make_unique<T[]>(capacity)), _capacity(capacity)
{
}

template<typename T>
T* Buffer<T>::begin()
{
    return buffer_ptr.get();
}

template<typename T>
T* Buffer<T>::end()
{
    return buffer_ptr.get() + _capacity;
}

template<typename T>
template<typename Iter>
size_t
Buffer<T>::read(Iter in, Iter end)
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

template<typename T>
template<typename Iter>
size_t
Buffer<T>::write(Iter out)
{
    size_t i;
    for(i = 0; i < _capacity; ++i)
    {
        *out = buffer_ptr[i];
        ++out;
    }
    return i;
}

template<typename T>
T &
Buffer<T>::operator[](size_t index)
{
    if(index >= _capacity) throw std::out_of_range("index out of range");
    return buffer_ptr[index];
}

template<typename T>
const T&
Buffer<T>::operator[](size_t index) const
{
    if(index >= _capacity) throw std::out_of_range("index out of range");
    return buffer_ptr[index];
}

template<typename T>
bool
Buffer<T>::operator==(const Buffer &other) const
{
    if(other._capacity != _capacity) return false;

    for(size_t i = 0; i < _capacity; ++i)
    {
        if(other[i] != operator[](i)) return false;
    }

    return true;
}

template<typename T>
void
Buffer<T>::resize(size_t new_capacity)
{
    std::unique_ptr<T[]> new_buffer = std::make_unique<T[]>(new_capacity);

    if(buffer_ptr != nullptr)
        std::copy(buffer_ptr.get(), buffer_ptr.get() + std::min(_capacity, new_capacity), new_buffer.get());

    buffer_ptr = new_buffer;
    _capacity = new_capacity;
}

template<typename T>
size_t
Buffer<T>::capacity()
{
    return _capacity;
}

template<typename T>
ByteString
Buffer<T>::to_bytestring()
{
    ByteString rv(std::move(buffer_ptr), _capacity);

    _capacity = 0;
    buffer_ptr = nullptr;

    return rv;
}

template <typename T>
std::unique_ptr<T[]>&
Buffer<T>::get()
{
    return buffer_ptr;
}