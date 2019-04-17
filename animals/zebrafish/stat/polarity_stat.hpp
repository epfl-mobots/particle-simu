#ifndef POLARITY_STAT_HPP
#define POLARITY_STAT_HPP

#include <fish_simulation.hpp>
#include <cmath>
#include <iomanip>

#include <stat/stat_base.hpp>

namespace simu {
    namespace stat {
        using namespace simulation;

        class PolarityStat : public StatBase {
        public:
            void operator()(const SimulationPtr sim) override
            {
                using namespace simulation;
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);

                if (!fsim->stats_enabled())
                    return;

                this->_create_log_file(fsim, "polarities.dat");

                if (fsim->iteration() == 0)
                    *_log_file << "#iteration polarity" << std::endl;

                int num_agents = fsim->fish_sim_settings().num_agents;
                float sum_heading = 0.0f;
                const auto& fish = fsim->fish();
                for (const auto& f : fish)
                    sum_heading += static_cast<float>(f->heading());
                sum_heading = std::abs(sum_heading);
                float polarity = sum_heading / num_agents;

                *_log_file << fsim->iteration() << " ";
                *_log_file << std::setprecision(8) << polarity << std::endl;
            }
        };

    } // namespace stat
} // namespace simu

#endif