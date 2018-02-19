#ifndef POLARITY_STAT_HPP
#define POLARITY_STAT_HPP

#include <cmath>
#include <iomanip>
#include <stat/stat_base.hpp>

namespace stat {
    template <typename Params> class PolarityStat : public StatBase<Params> {
    public:
        template <typename Simu> void operator()(const Simu& s)
        {
            if (!s->stats_enabled())
                return;

            this->_create_log_file(s, "polarities.dat");

            if (s->iteration() == 0)
                (*this->log_file_) << "#iteration polarity" << std::endl;

            size_t num_agents = s->num_agents();
            int sum_heading = 0;
            const auto& fish = s->fish();
            for (const auto& f : fish)
                sum_heading += static_cast<int>(f.heading());
            sum_heading = std::abs(sum_heading);
            double polarity = sum_heading / static_cast<double>(num_agents);

            (*this->log_file_) << s->iteration() << " ";
            (*this->log_file_) << std::setprecision(8) << polarity << std::endl;
        }
    };
} // namespace stat

#endif