#include <fish/zebrafish.hpp>
#include <simulation/fish_in_ring_sim.hpp>
#include <stat/fish_in_ring_params_stat.hpp>
#include <stat/heading_stat.hpp>
#include <stat/polarity_stat.hpp>
#include <stat/position_stat.hpp>
#include <tools/timer.hpp>

#include <iostream>

using namespace samsar;

struct Params {
    struct fish_in_ring : defaults::fish_in_ring {
    };

    struct simulation : defaults::simulation {
        static constexpr uint64_t sim_time = 1800;
    };
};

int main()
{
    using fish_type_t = fish::Zebrafish<Params>;
    using stat_t = boost::fusion::vector<stat::PositionStat<Params>, stat::HeadingStat<Params>,
        stat::PolarityStat<Params>, stat::FishInRingParamsStat<Params>>;
    sim::FishInRing<Params, fish_type_t, stat_t> fish_in_ring;

    tools::Timer t;
    t.start();
    fish_in_ring.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;

    return 0;
}
