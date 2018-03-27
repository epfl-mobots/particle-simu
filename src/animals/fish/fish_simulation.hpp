#ifndef FISH_SIMULATION_HPP
#define FISH_SIMULATION_HPP

#include "fish_individual.hpp"
#include <simulation/simulation.hpp>

namespace samsar {
    namespace simulation {

        class FishSimulation : public Simulation {
        public:
            FishSimulation(bool stats_enable = true);
            FishSimulation(const Settings& settings);
            FishSimulation(const Settings& settings, const std::vector<FishIndividual>& fish);

            virtual void spin_once() override;

            std::vector<FishIndividual> fish() const;
            std::vector<FishIndividual>& fish();

        private:
            void _init();

            std::vector<FishIndividual> _fish;
        };

        using FishSimulationPtr = std::shared_ptr<FishSimulation>;

    } // namespace simulation
} // namespace samsar

#endif
