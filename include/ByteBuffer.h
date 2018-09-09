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
     * Converts this buffer to a ByteString. This method effectively clears the buffer, as data is moved not copied.
     * New memory of the same size is allocated.
     *
     * @return A ByteString containing the Buffer's data
     */
    ByteString
    to_bytestring();
};