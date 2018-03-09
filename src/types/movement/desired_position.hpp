#ifndef DESIRED_POSITION_HPP
#define DESIRED_POSITION_HPP

#include <types/movement/heading.hpp>

namespace samsar {
    namespace types {
        struct DesiredPositionInfo {
            DesiredPositionInfo() : desired_position_(-1), heading_(Heading::UNDEFINED) {}
            DesiredPositionInfo(int dpos, Heading hdg) : desired_position_(dpos), heading_(hdg) {}

            void clear()
            {
                desired_position_ = -1;
                heading_ = Heading::UNDEFINED;
            }

            bool empty() const { return ((desired_position_ == -1) && (heading_ == Heading::UNDEFINED)); }

            int desired_position_;
            Heading heading_;
        };
    } // namespace types
} // namespace samsar

#endif
