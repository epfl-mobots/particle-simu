#include "simulation.hpp"
#include <tools/tools.hpp>

namespace simu {
    namespace simulation {

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
            while (_iteration < _sim_settings.sim_time) {
                spin_once();
            }
        }

        void Simulation::spin_once() { ++_iteration; }

        void Simulation::_update_stats(std::shared_ptr<Simulation> sim) const
        {
            if (!stats_enabled())
                return;

            for (const auto& stat : _stats)
                (*stat)(sim);
        }

        void Simulation::_update_descriptors(std::shared_ptr<Simulation> sim) const
        {
            for (const auto& desc : _descriptors)
                (*desc)(sim);
        }

        const std::string& Simulation::res_dir() const { return _res_dir; }

        uint64_t Simulation::sim_time() const { return _sim_settings.sim_time; }
        uint64_t& Simulation::sim_time() { return _sim_settings.sim_time; }

        uint64_t Simulation::iteration() const { return _iteration; }

        bool Simulation::stats_enabled() const { return _sim_settings.stats_enabled; }

        bool& Simulation::stats_enabled() { return _sim_settings.stats_enabled; }

        Simulation& Simulation::add_stat(const stat::StatBasePtr stat)
        {
            _stats.push_back(stat);
            return *this;
        }

        Simulation& Simulation::add_desc(const desc::DescriptorBasePtr desc)
        {
            _descriptors.push_back(desc);
            return *this;
        }

        Settings Simulation::sim_settings() const { return _sim_settings; }
        Settings& Simulation::sim_settings() { return _sim_settings; }

        std::vector<std::shared_ptr<stat::StatBase>> Simulation::stats() { return _stats; }
        std::vector<std::shared_ptr<desc::DescriptorBase>> Simulation::descriptors()
        {
            return _descriptors;
        }

    } // namespace simulation
} // namespace simu
