//
// Created by avris on 9/17/2018.
//

#include <sockets/Byte.h>

std::ostream& operator<<(std::ostream& out, ByteBuffer b)
{
    out << "ByteBuffer size=" << b.size() << " capacity=" << b.capacity() << ":";
    for(auto& c : b) out << c;
    return out;
}