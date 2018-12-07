//
// Created by avris on 10/22/2018.
//

#pragma once

#include <memory>
#include <exception>

namespace sockets {
    namespace abl {
        namespace win32 {
            class WSAError : public std::exception
            {
            public:
                const int error_code;
                const std::string function;

                WSAError(int error_code, std::string function);

                const char* what() const noexcept override;
            };


            /**
             * Loads the Winsock DLL on construction, and unloads it on deconstruction.
             */
            class WinSockDLL {
            private:
                struct Imp;
                std::unique_ptr<Imp> _imp;
            public:
                explicit WinSockDLL(unsigned char hi = 2, unsigned char low = 2);
                ~WinSockDLL();
            };
        }
    }
}