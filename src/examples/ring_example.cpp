#include <simulation/fish_in_ring_sim.hpp>
#include <stat/fish_in_ring_params_stat.hpp>
#include <stat/heading_stat.hpp>
#include <stat/polarity_stat.hpp>
#include <stat/position_stat.hpp>
#include <tools/timer.hpp>

#include <iostream>

#define USE_GROUP_LOGIC // TODO: temporary, remove after debugging
#ifndef USE_GROUP_LOGIC
#include <fish/zebrafish.hpp>
#else
#include <fish/group_zebrafish.hpp>
#endif

using namespace samsar;

struct Params {
    struct fish_in_ring {
        static constexpr size_t num_fish = 6;
        static constexpr size_t num_robot = 0;

        static constexpr int max_neighbors = 3;
        static constexpr size_t num_cells = 40;
        static constexpr int deg_vision = 120;
        static constexpr types::Heading heading_robot = types::Heading::CLOCKWISE;

        static constexpr float prob_obedience = 0.9f;
        static constexpr float prob_stay = 0.901f;
        static constexpr float prob_move = 1.0f;

        static constexpr size_t group_threshold = 2;
        static constexpr size_t group_cells_forward = 1;
        static constexpr size_t group_cells_backward = 1;
        static constexpr float social_influence = 1.0f;
    };

    struct simulation : defaults::simulation {
        static constexpr uint64_t sim_time = 1800;
    };
};

int main()
{
#ifndef USE_GROUP_LOGIC
    using fish_type_t = fish::Zebrafish<Params>;
#else
    using fish_type_t = fish::GroupZebrafish<Params>;
#endif
    using stat_t = boost::fusion::vector<stat::PositionStat<Params>, stat::HeadingStat<Params>,
        stat::PolarityStat<Params>, stat::FishInRingParamsStat<Params>>;
    sim::FishInRing<Params, fish_type_t, stat_t> fish_in_ring;
    //    fish_in_ring.spin_once();
    tools::Timer t;
    t.start();
    fish_in_ring.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;
    return 0;
}
