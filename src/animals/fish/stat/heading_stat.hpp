#ifndef HEADING_STAT_HPP
#define HEADING_STAT_HPP

#include <animals/fish/fish_simulation.hpp>
#include <stat/stat_base.hpp>

namespace samsar {
    namespace stat {
        using namespace simulation;

        class HeadingStat : public StatBase {
        public:
            void operator()(const SimulationPtr sim) override
            {
                using namespace simulation;
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);

                if (!fsim->stats_enabled())
                    return;

                this->_create_log_file(fsim, "headings.dat");

                if (fsim->iteration() == 0)
                    *_log_file << "#iteration fish1_heading fish2_heading ..." << std::endl;

                *_log_file << fsim->iteration() << " ";
                for (const auto& f : fsim->fish())
                    *_log_file << f->heading() << " ";
                *_log_file << std::endl;
            }
        };

    } // namespace stat
} // namespace samsar

#endif
