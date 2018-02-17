#ifndef TYPES_HPP
#define TYPES_HPP

#include <cmath>
#include <cstdint>
#include <string>

namespace types {
    enum Heading : int8_t { UNDEFINED = 0, CLOCKWISE = -1, COUNTER_CLOCKWISE = 1 };
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

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    Heading to_heading(T candidate)
    {
        if (candidate == 0)
            return Heading::UNDEFINED;
        else if (std::signbit(candidate))
            return Heading::CLOCKWISE;
        else
            return Heading::COUNTER_CLOCKWISE;
    }

    Heading reverse_heading(Heading hdg)
    {
        if (hdg == Heading::UNDEFINED)
            return Heading::UNDEFINED;
        else if (hdg == Heading::COUNTER_CLOCKWISE)
            return Heading::CLOCKWISE;
        else
            return Heading::COUNTER_CLOCKWISE;
    }
} // namespace types

#endif
