//
// Created by avris on 10/14/2018.
//

#pragma once

#include "Byte.h"
#include <type_traits>
#include <cstddef>

namespace sockets {
    /**
     * Tests if T implements size_t ::recv(ByteBuffer&, size_t, size_t, int)
     * @tparam T
     */
    template<typename T>
    struct has_recv
    {
    private:
        static ByteBuffer test_b;

        template<typename U>
        static auto test(size_t) -> decltype(std::declval<U>().recv(test_b, size_t(1), size_t(1), int(1)) == 1, std::true_type());

        template<typename>
        static std::false_type test(...);
    public:

        static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
    };

    template<typename T>
    struct has_send
    {
    private:
        template<typename U>
        static auto test(size_t) -> decltype(std::declval<U>().send(ByteBuffer(), size_t(1), size_t(1), int(1)) == 1, std::true_type);

        template<typename>
        static std::false_type test(...);
    public:

        static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
    };

    template<typename T>
    struct can_be_invalid
    {
    private:
        template<typename U>
        static auto test(size_t) -> decltype(std::declval<U>().invalid() == true, std::true_type);

        template<typename>
        static std::false_type test(...);

    public:

        static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
    };
}