#ifndef HEADING_HPP
#define HEADING_HPP

#include <random/random_generator.hpp>

#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace samsar {
    namespace types {
        enum Heading : int { COUNTER_CLOCKWISE = -1, UNDEFINED = 0, CLOCKWISE = 1 };

        std::string to_str(Heading heading)
        {
            switch (heading) {
            case Heading::CLOCKWISE:
                return "Clockwise";
            case Heading::COUNTER_CLOCKWISE:
                return "Counter-Clockwise";
            default:
                return "Undefined";
            }
        }

        template <typename T> Heading to_heading(T candidate)
        {
            if (candidate == 0)
                return Heading::UNDEFINED;
            else if (candidate < 0)
                return Heading::COUNTER_CLOCKWISE;
            else
                return Heading::CLOCKWISE;
        }

        Heading reverse_heading(Heading hdg) { return to_heading(-1 * static_cast<int>(hdg)); }

        bool is_same_heading(Heading hdg1, Heading hdg2)
        {
            int prod = hdg1 * hdg2;
            if (prod > 0)
                return true;
            else
                return false;
        }

        Heading random_heading() { return to_heading(tools::random_sgn()); }

    } // namespace types
} // namespace samsar

#endif
