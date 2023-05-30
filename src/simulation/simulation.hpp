#ifndef SIMU_SIMULATION_SIMULATION_HPP
#define SIMU_SIMULATION_SIMULATION_HPP

#include <descriptors/descriptor_base.hpp>
#include <stat/stat_base.hpp>
#include <vector>
#include <tools/tools.hpp>

namespace simu {
    namespace simulation {

        struct Settings {
            uint64_t sim_time = 1000;
            bool stats_enabled = true;
            float timestep = 0.12;
        };

        class Simulation {
        public:
            Simulation(const Settings& settings);
            virtual ~Simulation();

            virtual void spin();
            virtual void spin_once();

            const std::string& res_dir() const;
            float timestep() const;
            uint64_t sim_time() const;
            uint64_t& sim_time();
            uint64_t iteration() const;
            bool stats_enabled() const;
            bool& stats_enabled();
            Settings sim_settings() const;
            Settings& sim_settings();
            Simulation& add_stat(const std::shared_ptr<stat::StatBase> stat);
            Simulation& add_desc(const desc::DescriptorBasePtr desc);

            std::vector<std::shared_ptr<stat::StatBase>> stats();
            std::vector<std::shared_ptr<desc::DescriptorBase>> descriptors();

        protected:
            void _init();
            void _update_stats(const std::shared_ptr<Simulation>) const;
            void _update_descriptors(std::shared_ptr<Simulation> sim) const;

            Settings _sim_settings;
            std::vector<std::shared_ptr<stat::StatBase>> _stats;
            std::vector<std::shared_ptr<desc::DescriptorBase>> _descriptors;
            uint64_t _iteration;
            std::string _res_dir;
        };

        using SimulationPtr = std::shared_ptr<Simulation>;
        using SimulationConstPtr = std::shared_ptr<const Simulation>;
    } // namespace simulation
} // namespace simu

#endif
