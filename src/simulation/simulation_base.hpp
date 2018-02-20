#ifndef SIMULATION_BASE_HPP
#define SIMULATION_BASE_HPP

#include <tools/tools.hpp>

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/parameter.hpp>

#include <cstdint>
#include <stat/stat_base.hpp>
#include <string>

namespace defaults {
    struct simulation {
        static constexpr uint64_t sim_time = 1000;
        static constexpr bool stats_enabled = true;
    };
} // namespace defaults

namespace sim {

    template <typename Simu> struct RefreshStat {
        RefreshStat(const Simu& s) : simu_(s) {}
        template <typename T> void operator()(T& x) const { x(simu_); }
        const Simu& simu_;
    };

    template <typename Params, typename Stat> class SimulationBase {

        using base_type = SimulationBase<Params, Stat>;
        using stat_t =
            typename boost::mpl::if_<boost::fusion::traits::is_sequence<Stat>, Stat, boost::fusion::vector<Stat>>::type;

    public:
        SimulationBase()
            : sim_time_(Params::simulation::sim_time),
              stats_enabled_(Params::simulation::stats_enabled),
              iteration_(0),
              sim_start_stamp_(tools::timestamp()),
              res_dir_(tools::create_folder("sim_" + sim_start_stamp_))
        {
        }

        virtual void spin()
        {
            for (uint64_t i = 0; i < sim_time_; ++iteration_, ++i)
                spin_once();
        }

        virtual void spin_once() = 0;

        int iteration() const { return iteration_; }
        uint64_t sim_time() const { return sim_time_; }
        bool stats_enabled() const { return stats_enabled_; }
        bool& stats_enabled() { return stats_enabled_; }
        const std::string sim_start_stamp() const { return sim_start_stamp_; }
        const std::string res_dir() const { return res_dir_.c_str(); }

    protected:
        template <typename Simu> void update_stats(const Simu& s)
        {
            if (this->stats_enabled()) {
                boost::fusion::for_each(this->stat_, RefreshStat<Simu>(s));
            }
        }

    private:
        const uint64_t sim_time_;
        bool stats_enabled_;
        int iteration_;
        stat_t stat_;
        const std::string sim_start_stamp_;
        const boost::filesystem::path res_dir_;
    };
} // namespace sim

#endif
