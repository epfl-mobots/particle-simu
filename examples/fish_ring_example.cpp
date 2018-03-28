#include <iostream>

#include <animals/fish/fish_individual.hpp>
#include <animals/fish/fish_simulation.hpp>
#include <tools/timer.hpp>

#include <animals/fish/stat/fish_in_ring_params_stat.hpp>
#include <animals/fish/stat/group_params_stat.hpp>
#include <animals/fish/stat/group_stat.hpp>
#include <animals/fish/stat/heading_stat.hpp>
#include <animals/fish/stat/polarity_stat.hpp>
#include <animals/fish/stat/position_stat.hpp>

int main()
{
    using namespace samsar;
    using namespace simulation;
    using namespace stat;

    int num_cells = 40;
    int num_robot = 0;
    int num_fish = 6;

    Settings sim_set;
    sim_set.add_setting("stats_enabled", true)
        .add_setting("num_fish", num_fish)
        .add_setting("num_robot", num_robot)
        .add_setting("num_cells", num_cells)
        .add_setting("sim_time", 5400);

    FishSimulation sim(sim_set);
    sim.add_stat(std::make_shared<PositionStat>())
        .add_stat(std::make_shared<PolarityStat>())
        .add_stat(std::make_shared<PositionStat>())
        .add_stat(std::make_shared<HeadingStat>())
        .add_stat(std::make_shared<GroupStat>())
        .add_stat(std::make_shared<GroupParamsStat>())
        .add_stat(std::make_shared<FishInRingParamsStat>());

    // simply for testing purposes
    // TODO: remove in future versions
//#define TESTING
#ifdef TESTING
    for (auto& f : sim.fish()) {
        Settings fish_set;
        fish_set.add_setting("prob_obey", 1.0f)
            .add_setting("prob_move", 0.9f)
            .add_setting("min_speed", 1)
            .add_setting("max_speed", 1)
            .add_setting("group_threshold", 3)
            .add_setting("cells_forward", 5)
            .add_setting("cells_backward", 5)
            .add_setting("sum_weight", std::vector<float>{0.3f, -2.f})
            .add_setting("heading_robot", Heading::CLOCKWISE)
            .add_setting("heading_change_duration", 2);
        f.individual_settings() = fish_set;
        f.force_init();
    }
#endif

    tools::Timer t;
    t.start();
    sim.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;

    return 0;
}
