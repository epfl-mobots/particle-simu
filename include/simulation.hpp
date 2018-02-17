#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstdint>

namespace sim {
    template <typename Params> class SimulatorBase {
    public:
        SimulatorBase() : sim_time_(Params::simulation::sim_time) {}

        virtual void spin()
        {
            for (uint64_t i = 0; i < sim_time_; ++i)
                spin_once();
        }

        virtual void spin_once() = 0;

        uint64_t& sim_time() { return sim_time_; }
        uint64_t sim_time() const { return sim_time_; }

    private:
        uint64_t sim_time_;
    };
} // namespace sim

#endif
