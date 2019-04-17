#ifndef SIMU_TOOLS_MATH_HPP
#define SIMU_TOOLS_MATH_HPP

namespace simu {
    namespace tools {

        template <typename T>
        int sgn(T val) { return (T(0) < val) - (val < T(0)); }

    } // namespace tools
} // namespace simu

#endif
