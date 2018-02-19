#ifndef FISH_IN_RING_PARAMS_STAT_HPP
#define FISH_IN_RING_PARAMS_STAT_HPP

#include <iomanip>
#include <stat/stat_base.hpp>
#include <types/types.hpp>

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
                (*this->log_file_) << "Number of fish: " << Params::fish_in_ring::num_fish << std::endl;
                (*this->log_file_) << "Number of robots: " << Params::fish_in_ring::num_robot << std::endl;
                (*this->log_file_) << "Max neighbors: " << Params::fish_in_ring::max_neighbors << std::endl;
                (*this->log_file_) << "Number of cells: " << Params::fish_in_ring::num_cells << std::endl;
                (*this->log_file_) << "Degrees of vision: " << Params::fish_in_ring::deg_vision << std::endl;
                (*this->log_file_) << "Robot heading: " << to_str(Params::fish_in_ring::heading_robot) << std::endl;
                (*this->log_file_) << "Probability to obey: " << Params::fish_in_ring::prob_obedience << std::endl;
                (*this->log_file_) << "Probability to stay in position (fish): " << Params::fish_in_ring::prob_stay
                                   << std::endl;
                (*this->log_file_) << "Probability to move (robot): " << Params::fish_in_ring::prob_move << std::endl;
            }
        }
    };
} // namespace stat

#endif
