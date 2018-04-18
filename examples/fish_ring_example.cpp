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

int to_discrete(double val, int upper_lim, int lower_lim = 0)
{
    return static_cast<int>(std::floor(val * (upper_lim - lower_lim) + lower_lim));
}

int main()
{
    using namespace samsar;
    using namespace simulation;
    using namespace stat;

    int num_cells = 40;
    int num_robot = 1;
    int num_fish = 5;

    FishSimSettings set;
    set.sim_time = 5400;
    set.stats_enabled = true;
    set.num_fish = num_fish;
    set.num_robot = num_robot;
    set.num_cells = num_cells;

    std::vector<FishIndividualPtr> fish;
    FishIndividualPtr agent = std::make_shared<FishIndividual>();
    agent->is_robot() = true;
    agent->id() = 0;
    fish.push_back(agent);

    FishParams params;
    params.prob_obey = 1.0f;
    params.prob_move = 0.901f;
    params.group_threshold = 3;
    params.cells_forward = 5;
    params.cells_backward = 5;
    params.sum_weight = {0.3f, -2.0f};
    params.influence_alpha = 4;
    params.heading_change_duration = 2;
    fish[0]->fish_params() = params;
    fish[0]->force_init();

    for (int i = 0; i < num_fish; ++i) {
        FishIndividualPtr def_agent = std::make_shared<FishIndividual>();
        def_agent->id() = i;
        fish.push_back(def_agent);
    }

    FishSimulation sim(set, fish);
    sim.add_stat(std::make_shared<PositionStat>())
        .add_stat(std::make_shared<PolarityStat>())
        .add_stat(std::make_shared<PositionStat>())
        .add_stat(std::make_shared<HeadingStat>())
        .add_stat(std::make_shared<GroupStat>())
        .add_stat(std::make_shared<GroupParamsStat>())
        .add_stat(std::make_shared<FishInRingParamsStat>());

    tools::Timer t;
    t.start();
    sim.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;

    return 0;
}
