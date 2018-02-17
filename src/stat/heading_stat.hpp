#ifndef HEADING_STAT_HPP
#define HEADING_STAT_HPP

#include <stat/stat_base.hpp>

namespace stat {

    template <typename Params> class HeadingStat : public StatBase<Params> {
    public:
        template <typename Simu> void operator()(const Simu& s)
        {
            if (!s->stats_enabled())
                return;

            this->_create_log_file(s, "headings.dat");

            if (s->iteration() == 0)
                (*this->log_file_) << "#iteration fish1_heading fish2_heading ..." << std::endl;

            (*this->log_file_) << s->iteration() << " ";
            for (const auto& f : s->fish())
                (*this->log_file_) << f.heading() << " ";
            (*this->log_file_) << std::endl;
        }
    };

} // namespace stat

#endif
