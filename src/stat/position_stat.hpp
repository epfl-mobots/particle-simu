#ifndef POSITION_STAT_HPP
#define POSITION_STAT_HPP

#include <stat/stat_base.hpp>

namespace samsar {
    namespace stat {
        template <typename Params> class PositionStat : public StatBase<Params> {
        public:
            template <typename Simu> void operator()(const Simu& s)
            {
                if (!s->stats_enabled())
                    return;

                this->_create_log_file(s, "positions.dat");

                if (s->iteration() == 0)
                    (*this->log_file_) << "#iteration fish1_position fish2_position ..." << std::endl;

                (*this->log_file_) << s->iteration() << " ";
                for (const auto& f : s->fish())
                    (*this->log_file_) << f.position() << " ";
                (*this->log_file_) << std::endl;
            }
        };
    } // namespace stat
} // namespace samsar

#endif
