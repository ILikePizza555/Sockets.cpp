//
// Created by avris on 9/10/2018.
//
#include "catch.hpp"

#include <algorithm>
#include <Connection.h>

/**
 * Outputs the number 10 on calls to recv(). If n > than amount, amount is respected. Ignore all other functions.
 */
template <size_t n>
struct ReceiveSocketStub
{
    ssize_t
    recv(ByteBuffer& b, size_t amount, int flags)
    {
        auto write_amt = std::min(amount, n);
        for(size_t i = 0; i < write_amt; ++i)
            b[i] = 10;
        return write_amt;
    }

    ssize_t
    recv(byte* b, size_t amount, int flags)
    {
        auto write_amt = std::min(amount, n);
        for(size_t i = 0; i < write_amt; ++i)
            *(b + i) = 10;
        return write_amt;
    }

    ssize_t
    send(const ByteBuffer& buffer, int flags)
    {
        return 0;
    }

    ssize_t
    send(const byte*, size_t length, int flags)
    {
        return 0;
    }

};