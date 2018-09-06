//
// Created by avris on 9/5/2018.
//

#include <Buffer.h>

template<typename T>
template<typename Iter>
size_t
Buffer<T>::read(Iter in, Iter end)
{
    size_t i;
    for(i = 0; i < capacity; ++i)
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
    for(i = 0; i < capacity; ++i)
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
    if(index >= capacity) throw std::out_of_range("index out of range");
    return buffer_ptr[index];
}


template<typename T>
bool
Buffer<T>::operator==(const Buffer &other) const
{
    if(other.capacity != capacity) return false;

    for(size_t i = 0; i < capacity; ++i)
    {
        if(other[i] != operator[](i)) return false;
    }

    return true;
}

