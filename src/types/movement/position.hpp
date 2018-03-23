#ifndef POSITION_HPP
#define POSITION_HPP

namespace samsar {
    namespace types {

        template <typename T = float> struct Position {
            Position() : x(0), y(0), z(0) {}

            T x;
            T y;
            T z;
        };

    } // namespace types
} // namespace samsar

#endif
