#ifndef FISH_SIMULATION_HPP
#define FISH_SIMULATION_HPP

#include "fish_individual.hpp"
#include <simulation/simulation.hpp>

namespace samsar {
    namespace simulation {

        struct FishSimSettings : public Settings {
            FishSimSettings()
            {
                sim_time = 5400;
                num_agents = num_fish + num_robot;
            }

            int num_fish = 5;
            int num_robot = 1;
            int num_cells = 40;

            int num_agents;
        };

        class FishSimulation : public Simulation {
        public:
            FishSimulation(bool stats_enable = true);
            FishSimulation(const FishSimSettings& settings);
            FishSimulation(
                const FishSimSettings& settings, const std::vector<FishIndividualPtr>& fish);

            virtual void spin_once() override;

            std::vector<FishIndividualPtr> fish() const;
            std::vector<FishIndividualPtr>& fish();

            FishSimSettings fish_sim_settings() const;
            FishSimSettings& fish_sim_settings();

        private:
            void _init();

            FishSimSettings _fish_sim_settings;
            std::vector<FishIndividualPtr> _fish;
        };

        using FishSimulationPtr = std::shared_ptr<FishSimulation>;

    } // namespace simulation
} // namespace samsar

#endif
