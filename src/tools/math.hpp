#ifndef MATH_HPP
#define MATH_HPP

namespace samsar {
    namespace tools {

        template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

    } // namespace tools
} // namespace samsar

#endif
