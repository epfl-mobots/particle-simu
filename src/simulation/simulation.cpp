#include "simulation.hpp"
#include <tools/tools.hpp>

namespace samsar {
    namespace simulation {

        Simulation::Simulation(bool stats_enabled)
        {
            _sim_settings.stats_enabled = stats_enabled;
            _init();
        }

        Simulation::Simulation(const Settings& settings)
        {
            _sim_settings = settings;
            _init();
        }

        Simulation::~Simulation() {}

        void Simulation::_init()
        {
            _iteration = 0;
            if (stats_enabled())
                _res_dir
                    = tools::create_folder(boost::asio::ip::host_name() + "_" + tools::timestamp())
                          .c_str();
        }

        void Simulation::spin()
        {
            for (uint64_t i = 0; i < sim_time(); ++i)
                spin_once();
        }

        void Simulation::spin_once() { ++_iteration; }

        void Simulation::_update_stats(std::shared_ptr<Simulation> sim) const
        {
            if (!stats_enabled())
                return;

            for (const auto& stat : _stats)
                (*stat)(sim);
        }

        const std::string& Simulation::res_dir() const { return _res_dir; }

        uint64_t Simulation::sim_time() const { return _sim_settings.sim_time; }

        uint64_t Simulation::iteration() const { return _iteration; }

        bool Simulation::stats_enabled() const { return _sim_settings.stats_enabled; }

        bool& Simulation::stats_enabled() { return _sim_settings.stats_enabled; }

        Simulation& Simulation::add_stat(const stat::StatBasePtr stat)
        {
            _stats.push_back(stat);
            return *this;
        }

        Settings Simulation::sim_settings() const { return _sim_settings; }
        Settings& Simulation::sim_settings() { return _sim_settings; }

    } // namespace simulation
} // namespace samsar
