#include <fish/zebrafish.hpp>
#include <simulation/fish_in_ring_sim.hpp>
#include <stat/fish_in_ring_params_stat.hpp>
#include <stat/heading_stat.hpp>
#include <stat/polarity_stat.hpp>
#include <stat/position_stat.hpp>

#include <iostream>

struct Params {
    struct fish_in_ring {
        static constexpr size_t num_fish = 5;
        static constexpr size_t num_robot = 1;

        static constexpr int max_neighbors = 3;
        static constexpr size_t num_cells = 40;
        static constexpr int deg_vision = 120;
        static constexpr types::Heading heading_robot = types::Heading::COUNTER_CLOCKWISE;

        static constexpr float prob_obedience = 0.9f;
        static constexpr float prob_stay = 0.901f;
        static constexpr float prob_move = 1;
    };

    struct simulation : defaults::simulation {
        static constexpr uint64_t sim_time = 1800;
    };
};

int main()
{
    using stat_t = boost::fusion::vector<stat::PositionStat<Params>, stat::HeadingStat<Params>,
        stat::PolarityStat<Params>, stat::FishInRingParamsStat<Params>>;
    sim::FishInRing<Params, fish::Zebrafish<Params>, stat_t> fish_in_ring;
    //    fish_in_ring.spin_once();
    fish_in_ring.spin();
    return 0;
}
