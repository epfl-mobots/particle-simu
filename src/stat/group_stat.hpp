#ifndef GROUP_STAT_HPP
#define GROUP_STAT_HPP

#include <iomanip>
#include <stat/stat_base.hpp>
#include <types/types.hpp>

namespace samsar {
    namespace stat {
        template <typename Params> class GroupStat : public StatBase<Params> {
        public:
            template <typename Simu> void operator()(const Simu& s)
            {
                if (!s->stats_enabled())
                    return;

                this->_create_log_file(s, "group.dat");

                if (s->iteration() == 0)
                    (*this->log_file_) << "#iteration fish1_group_size fish2_group_size ..." << std::endl;

                (*this->log_file_) << s->iteration() << " ";
                for (const auto& f : s->fish())
                    (*this->log_file_) << f.group_size() << " ";
                (*this->log_file_) << std::endl;
            }
        };
    } // namespace stat
} // namespace samsar

#endif
