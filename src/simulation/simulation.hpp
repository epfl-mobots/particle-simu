#ifndef SIMULATION_BASE_HPP
#define SIMULATION_BASE_HPP

#include <settings/settings.hpp>
#include <stat/stat_base.hpp>
#include <vector>

namespace samsar {

    namespace simulation {

        class Simulation {
        public:
            Simulation();
            Simulation(const Settings& settings);
            virtual ~Simulation();

            virtual void spin();
            virtual void spin_once();

            const std::string& res_dir() const;
            uint64_t sim_time() const;
            uint64_t iteration() const;
            bool stats_enabled() const;
            bool& stats_enabled();
            Settings sim_settings() const;
            Settings& sim_settings();
            Simulation& add_stat(const std::shared_ptr<stat::StatBase> stat);

        protected:
            void _init();
            void _update_stats(const std::shared_ptr<Simulation>) const;

            Settings _sim_settings;
            std::vector<std::shared_ptr<stat::StatBase>> _stats;
            uint64_t _iteration;
            std::string _res_dir;
        };

        using SimulationPtr = std::shared_ptr<Simulation>;
        using SimulationConstPtr = std::shared_ptr<const Simulation>;
    } // namespace simulation
} // namespace samsar

#endif
