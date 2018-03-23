#ifndef FISH_IN_RING_PARAMS_STAT_HPP
#define FISH_IN_RING_PARAMS_STAT_HPP

#include <animals/fish/fish_simulation.hpp>
#include <iomanip>
#include <stat/stat_base.hpp>

namespace samsar {
    namespace stat {
        using namespace simulation;

        class FishInRingParamsStat : public StatBase {
        public:
            void operator()(const SimulationPtr sim) override
            {
                using namespace simulation;
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);

                if (!fsim->stats_enabled())
                    return;

                this->_create_log_file(fsim, "fish_in_ring_params.dat");

                if (fsim->iteration() == 0) {
                    *_log_file << "-- Simulation params --" << std::endl;
                    *_log_file << "Simulation time: "
                               << fsim->sim_settings().get_field<size_t>("sim_time")->value()
                               << std::endl;
                    *_log_file << "Number of fish: "
                               << fsim->sim_settings().get_field<size_t>("num_fish")->value()
                               << std::endl;
                    *_log_file << "Number of robots: "
                               << fsim->sim_settings().get_field<size_t>("num_robot")->value()
                               << std::endl;
                    *_log_file << "Number of cells: "
                               << fsim->sim_settings().get_field<size_t>("num_cells")->value()
                               << std::endl;
                    *_log_file << "Group dump start: 0" << std::endl;
                    //                    *_log_file << "Degrees of vision: "
                    //                               <<
                    //                               fsim->sim_settings().get_field<size_t>("deg_vision")->value()
                    //                               << std::endl;
                }
            }
        };
    } // namespace stat
} // namespace samsar

#endif
