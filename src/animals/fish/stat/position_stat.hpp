#ifndef POSITION_STAT_HPP
#define POSITION_STAT_HPP

#include <animals/fish/fish_simulation.hpp>
#include <stat/stat_base.hpp>

namespace samsar {
    namespace stat {
        using namespace simulation;

        class PositionStat : public StatBase {
        public:
            void operator()(const SimulationPtr sim) override
            {
                using namespace simulation;
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);

                if (!fsim->stats_enabled())
                    return;

                _create_log_file(fsim, "positions.dat");

                if (fsim->iteration() == 0)
                    *_log_file << "#iteration fish1_position fish2_position ..." << std::endl;

                *_log_file << fsim->iteration() << " ";
                for (const FishIndividualPtr& f : fsim->fish())
                    *_log_file << f->position().x << " ";
                *_log_file << std::endl;
            }
        };

    } // namespace stat
} // namespace samsar

#endif
