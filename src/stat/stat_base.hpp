#ifndef STAT_BASE_HPP
#define STAT_BASE_HPP

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace stat {

    template <typename Params> class StatBase {
    public:
        StatBase() {}

        template <typename Simu> void operator()(const Simu&) { assert(false); }

    protected:
        std::shared_ptr<std::ofstream> log_file_;

        template <typename Simu> void _create_log_file(const Simu& s, const std::string& filename)
        {
            if (!log_file_ && s->stats_enabled()) {
                log_file_ = std::make_shared<std::ofstream>(filename.c_str());
                assert(log_file_->good());
            }
        }
    };

} // namespace stat

#endif
