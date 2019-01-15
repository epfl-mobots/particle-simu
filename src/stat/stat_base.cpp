#include "stat_base.hpp"

#include <simulation/simulation.hpp>

namespace simu {
    namespace stat {

        StatBase::~StatBase() {}

        void StatBase::_create_log_file(
            const std::shared_ptr<simulation::Simulation> s, const std::string& filename)
        {
            if (!_log_file && s->stats_enabled()) {
                std::string abs_path = s->res_dir() + "/" + filename.c_str();
                _log_file = std::make_shared<std::ofstream>(abs_path);
                assert(_log_file->good());
            }
        }

    } // namespace stat
} // namespace simu
