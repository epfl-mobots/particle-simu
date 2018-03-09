#ifndef FISH_IN_RING_PARAMS_STAT_HPP
#define FISH_IN_RING_PARAMS_STAT_HPP

#include <iomanip>
#include <stat/stat_base.hpp>
#include <types/types.hpp>

namespace samsar {
    namespace stat {
        template <typename Params> class FishInRingParamsStat : public StatBase<Params> {
        public:
            template <typename Simu> void operator()(const Simu& s)
            {
                if (!s->stats_enabled())
                    return;

                this->_create_log_file(s, "fish_in_ring_params.dat");

                if (s->iteration() == 0) {
                    (*this->log_file_) << "-- Simulation params --" << std::endl;
                    (*this->log_file_) << "Simulation time: " << Params::simulation::sim_time << std::endl;
                    (*this->log_file_) << "Dump start time: " << Params::simulation::dump_start << std::endl;
                    (*this->log_file_) << "Number of fish: " << Params::ring::num_fish << std::endl;
                    (*this->log_file_) << "Number of robots: " << Params::ring::num_robot << std::endl;
                    (*this->log_file_) << "Number of cells: " << Params::ring::num_cells << std::endl;
                    (*this->log_file_) << "Degrees of vision: " << Params::ring::deg_vision << std::endl;
                }
            }
        };
    } // namespace stat
} // namespace samsar

#endif
