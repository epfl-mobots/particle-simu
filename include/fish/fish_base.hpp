#ifndef FISH_HPP
#define FISH_HPP

#include <random/random_generator.hpp>
#include <types/types.hpp>

namespace agent {
    using namespace types;

    template <typename Params> class FishBase {
    public:
        FishBase() : heading_(static_cast<Heading>(tools::random_sgn())), position_(-1), is_robot_(false) {}

        int& position() { return position_; }
        int position() const { return position_; }
        Heading& heading() { return heading_; }
        Heading heading() const { return heading_; }
        bool is_robot() const { return is_robot_; }
        void is_robot(bool flag) { is_robot_ = flag; }

    private:
        Heading heading_;
        int position_;
        bool is_robot_;
    };

} // namespace agent

#endif
