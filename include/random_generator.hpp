#ifndef RANDOM_GENERATOR_HPP
#define RANDOM_GENERATOR_HPP

#include <iostream>
#include <random>
#include <type_traits>

namespace tools {

    std::random_device random_device_;
    std::mt19937 mt(random_device_());

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
    T random_in_range(T lower_bound, T upper_bound)
    {
        std::uniform_real_distribution<T> distribution(lower_bound, upper_bound);
        return distribution(mt);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    T random_in_range(T lower_bound, T upper_bound)
    {
        std::uniform_int_distribution<T> distribution(lower_bound, upper_bound);
        return distribution(mt);
    }

    int8_t random_sgn()
    {
        if (random_in_range(0, 1) == 0)
            return 1;
        else
            return -1;
    }

} // namespace tools

#endif
