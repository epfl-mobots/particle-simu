#include "heading.hpp"

namespace samsar {
    namespace types {

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
