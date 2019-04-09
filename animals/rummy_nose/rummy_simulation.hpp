#ifndef RUMMY_SIMULATION_HPP
#define RUMMY_SIMULATION_HPP

#include "rummy_individual.hpp"
#include <simulation/simulation.hpp>

namespace simu {
    namespace simulation {

        class RummySimulation : public Simulation {
        public:
            RummySimulation(bool stats_enable = true);

            virtual void spin_once() override;

            std::vector<RummyIndividualPtr> fish() const;
            std::vector<RummyIndividualPtr>& fish();

        private:
            void _init();

            std::vector<RummyIndividualPtr> _fish;
        };

        using FishSimulationPtr = std::shared_ptr<RummySimulation>;

    } // namespace simulation
} // namespace simu

#endif
