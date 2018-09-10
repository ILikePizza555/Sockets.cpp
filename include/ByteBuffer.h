//
// Created by avris on 9/9/2018.
//

#pragma once

#include "Buffer.h"
#include "ByteString.h"

class ByteBuffer : public Buffer<byte>
{
public:
    using Buffer<byte>::Buffer;

    /**
     * Converts this buffer to a ByteString.
     *
     * Because reallocation is faster than copying, data is moved from the buffer to the bytestring, and then a new
     * space is allocated. Therefore, this operation effectively clears the buffer.
     *
     * @return A ByteString containing the Buffer's data
     */
    ByteString
    to_bytestring();
};