#ifndef GROUP_PARAMS_STAT_HPP
#define GROUP_PARAMS_STAT_HPP

#include <animals/fish/fish_simulation.hpp>
#include <iomanip>
#include <stat/stat_base.hpp>

namespace samsar {
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
                               << fsim->fish()
                                      .at(0)
                                      .fish_settings()
                                      .get_field<size_t>("group_threshold")
                                      ->value()
                               << std::endl;
                    *_log_file << "Group cells forward: "
                               << fsim->fish()
                                      .at(0)
                                      .fish_settings()
                                      .get_field<size_t>("cells_forward")
                                      ->value()
                               << std::endl;
                    *_log_file << "Group cells backward: "
                               << fsim->fish()
                                      .at(0)
                                      .fish_settings()
                                      .get_field<size_t>("cells_backward")
                                      ->value()
                               << std::endl;
                }
            }
        };

    } // namespace stat
} // namespace samsar

#endif
