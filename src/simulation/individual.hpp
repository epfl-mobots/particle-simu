#ifndef SIMU_SIMULATION_INDIVIDUAL_HPP
#define SIMU_SIMULATION_INDIVIDUAL_HPP

#include <simulation/simulation.hpp>
#include <types/movement/position.hpp>
#include <types/movement/speed.hpp>

#include <cassert>

namespace simu {
    namespace simulation {
        using namespace types;

        template <typename PositionType = float, typename SpeedType = float>
        class Individual {
        public:
            using position_type_t = PositionType;
            using speed_type_t = SpeedType;

            Individual() : _is_robot(false) {}

            virtual void stimulate(const std::shared_ptr<Simulation>) = 0;
            virtual void move(const std::shared_ptr<Simulation>) = 0;

            Position<PositionType> position() const { return _position; }
            Position<PositionType>& position() { return _position; }

            Speed<SpeedType> speed() const { return _speed; }
            Speed<SpeedType>& speed() { return _speed; }

            virtual int id() const { return _id; }
            virtual int& id() { return _id; }

            bool is_robot() const { return _is_robot; }
            bool& is_robot() { return _is_robot; }

        protected:
            Position<PositionType> _position;
            Speed<SpeedType> _speed;

            int _id;
            bool _is_robot;
        };

    } // namespace simulation
} // namespace simu

#endif
