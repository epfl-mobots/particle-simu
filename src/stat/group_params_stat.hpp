#ifndef GROUP_PARAMS_STAT_HPP
#define GROUP_PARAMS_STAT_HPP

#include <iomanip>
#include <stat/stat_base.hpp>
#include <types/types.hpp>

namespace samsar {
    namespace stat {
        template <typename Params> class GroupParamsStat : public StatBase<Params> {
        public:
            template <typename Simu> void operator()(const Simu& s)
            {
                if (!s->stats_enabled())
                    return;

                this->_create_log_file(s, "group_params.dat");

                if (s->iteration() == 0) {
                    (*this->log_file_) << "-- Group params --" << std::endl;
                    (*this->log_file_) << "Group threshold: " << Params::social_zebrafish::group_threshold << std::endl;
                    (*this->log_file_) << "Group cells forward: " << Params::social_zebrafish::cells_forward
                                       << std::endl;
                    (*this->log_file_) << "Group cells backward: " << Params::social_zebrafish::cells_backward
                                       << std::endl;
                }
            }
        };
    } // namespace stat
} // namespace samsar

#endif
