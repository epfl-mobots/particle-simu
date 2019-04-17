#ifndef GROUP_PARAMS_STAT_HPP
#define GROUP_PARAMS_STAT_HPP

#include <fish_simulation.hpp>
#include <iomanip>
#include <stat/stat_base.hpp>

namespace simu {
    namespace stat {
        using namespace simulation;

        class GroupParamsStat : public StatBase {
        public:
            void operator()(const SimulationPtr sim) override
            {
                using namespace simulation;
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);

                if (!fsim->stats_enabled())
                    return;
                this->_create_log_file(fsim, "group_params.dat");

                if (fsim->iteration() == 0) {
                    *_log_file << "-- Group params --" << std::endl;
                    *_log_file << "Group threshold: "
                               << fsim->fish().at(0)->fish_params().group_threshold << std::endl;
                    *_log_file << "Group cells forward: "
                               << fsim->fish().at(0)->fish_params().cells_forward << std::endl;
                    *_log_file << "Group cells backward: "
                               << fsim->fish().at(0)->fish_params().cells_backward << std::endl;
                }
            }
        };

    } // namespace stat
} // namespace simu

#endif