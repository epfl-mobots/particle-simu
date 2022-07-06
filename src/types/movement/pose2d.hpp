#ifndef SIMU_TYPES_POSE2D_HPP
#define SIMU_TYPES_POSE2D_HPP

namespace simu {
    namespace types {

        template <typename T = float>
        struct Pose2d {
            Pose2d() : x(0), y(0), yaw(0) {}

            T x;
            T y;
            T yaw;
        };

    } // namespace types
} // namespace simu

#endif
