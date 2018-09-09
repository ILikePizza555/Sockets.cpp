//
// Created by avris on 9/9/2018.
//

#include <ByteBuffer.h>

ByteString
ByteBuffer::to_bytestring()
{
    ByteString rv = ByteString(std::move(Buffer<byte>::buffer_ptr), Buffer<byte>::_capacity);
    Buffer<byte>::buffer_ptr = std::make_unique<byte[]>(Buffer<byte>::_capacity);
    return rv;
}
