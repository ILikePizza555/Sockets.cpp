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
