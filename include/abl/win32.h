//
// Created by avris on 10/22/2018.
//

#pragma once

#include <memory>

namespace sockets {
    namespace abl {
        namespace win32 {

            /**
             * Loads the Winsock DLL on construction, and unloads it on deconstruction.
             */
            class WinSockDLL {
            private:
                struct WSAData;
                std::unique_ptr<WSAData> _data;
            public:
                explicit WinSockDLL(unsigned char hi = 2, unsigned char low = 2);
                ~WinSockDLL();
            };
        }
    }
}