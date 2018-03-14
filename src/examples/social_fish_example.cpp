#include <fish/social_zebrafish.hpp>
#include <simulation/fish_in_ring.hpp>
#include <stat/fish_in_ring_params_stat.hpp>
#include <stat/group_params_stat.hpp>
#include <stat/group_stat.hpp>
#include <stat/heading_stat.hpp>
#include <stat/polarity_stat.hpp>
#include <stat/position_stat.hpp>
#include <tools/timer.hpp>

#include <iostream>

using namespace samsar;

struct Params {
    struct ring : public defaults::ring {
        static constexpr size_t num_fish = 6;
        static constexpr size_t num_robot = 0;
        static constexpr size_t num_cells = 160;
    };

    struct social_zebrafish : public defaults::social_zebrafish {
    };

    struct simulation : public defaults::simulation {
        static constexpr uint64_t sim_time = 7200;
    };
};

int main()
{
    using fish_type_t = fish::SocialZebrafish<Params>;
    using stat_t
        = boost::fusion::vector<stat::PositionStat<Params>, stat::HeadingStat<Params>, stat::PolarityStat<Params>,
            stat::FishInRingParamsStat<Params>, stat::GroupParamsStat<Params>, stat::GroupStat<Params>>;
    sim::FishInRing<Params, fish_type_t, stat_t> fish_in_ring;

    int random_pos = static_cast<int>(tools::random_in_range(0.0f, 1.0f) * fish_in_ring.num_cells());
    for (auto& f : fish_in_ring.fish()) {
        int sgn = tools::random_sgn();
        int pos;
        (sgn < 0) ? pos = tools::random_in_range(0, static_cast<int>(Params::social_zebrafish::cells_forward))
                  : pos = tools::random_in_range(0, static_cast<int>(Params::social_zebrafish::cells_backward));
        f.position() = tools::random_in_range(0, pos) + random_pos;
        f.heading() = types::Heading::CLOCKWISE;
    }

    tools::Timer t;
    t.start();
    fish_in_ring.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;

    return 0;
}
