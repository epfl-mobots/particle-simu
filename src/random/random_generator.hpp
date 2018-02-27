#ifndef RANDOM_GENERATOR_HPP
#define RANDOM_GENERATOR_HPP

#include <iostream>
#include <random/rand_utils.hpp>
#include <random>
#include <type_traits>

namespace samsar {
    namespace tools {

        class RandomGenerator {
        public:
            RandomGenerator() : _rgen(randutils::auto_seed_128{}.base()) {}
            std::mt19937 gen() const { return _rgen; }

        private:
            std::mt19937 _rgen;
        };

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
        T random_in_range(T lower_bound, T upper_bound)
        {
            std::uniform_real_distribution<T> distribution(lower_bound, upper_bound);
            auto gen = RandomGenerator().gen();
            return distribution(gen);
        }

        template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
        T random_in_range(T lower_bound, T upper_bound)
        {
            std::uniform_int_distribution<T> distribution(lower_bound, upper_bound);
            auto gen = RandomGenerator().gen();
            return distribution(gen);
        }

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
        T normal_random_in_range(T lower_bound, T upper_bound)
        {
            std::normal_distribution<T> distribution(lower_bound, upper_bound);
            auto gen = RandomGenerator().gen();
            return distribution(gen);
        }

        int8_t random_sgn()
        {
            if (random_in_range(0, 1) == 0)
                return 1;
            else
                return -1;
        }

    } // namespace tools
} // namespace samsar

#endif
