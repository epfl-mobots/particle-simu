#ifndef SPEED_HPP
#define SPEED_HPP

namespace samsar {
    namespace types {

        template <typename T = float> struct Speed {
            Speed() : current(0), min_speed(0), max_speed(0) {}

            bool valid() const { return (current >= min_speed) && (current <= max_speed); }

            T current;
            T min_speed;
            T max_speed;
        };

    } // namespace types
} // namespace samsar

#endif
