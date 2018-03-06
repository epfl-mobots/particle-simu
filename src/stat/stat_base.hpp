#ifndef STAT_BASE_HPP
#define STAT_BASE_HPP

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace samsar {
    namespace stat {
        template <typename Params> class StatBase {
        public:
            StatBase() {}

            template <typename Simu> void operator()(const Simu&) {}

        protected:
            std::shared_ptr<std::ofstream> log_file_;

            template <typename Simu> void _create_log_file(const Simu& s, const std::string& filename)
            {
                if (!log_file_ && s->stats_enabled()) {
                    std::string abs_path = s->res_dir() + "/" + filename.c_str();
                    log_file_ = std::make_shared<std::ofstream>(abs_path);
                    assert(log_file_->good());
                }
            }
        };
    } // namespace stat
} // namespace samsar

#endif
