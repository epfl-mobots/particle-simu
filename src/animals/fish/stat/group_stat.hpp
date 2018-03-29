#ifndef GROUP_STAT_HPP
#define GROUP_STAT_HPP

#include <animals/fish/fish_simulation.hpp>
#include <iomanip>
#include <stat/stat_base.hpp>

namespace samsar {
    namespace stat {
        using namespace simulation;

        class GroupStat : public StatBase {
        public:
            void operator()(const SimulationPtr sim) override
            {
                using namespace simulation;
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);

                if (!fsim->stats_enabled())
                    return;

                this->_create_log_file(fsim, "group.dat");

                if (fsim->iteration() == 0)
                    *_log_file << "#iteration fish1_group_size fish2_group_size ..." << std::endl;

                *_log_file << fsim->iteration() << " ";
                for (const auto& f : fsim->fish())
                    *_log_file << f->group_size() << " ";
                *_log_file << std::endl;
            }
        };

    } // namespace stat
} // namespace samsar

#endif
