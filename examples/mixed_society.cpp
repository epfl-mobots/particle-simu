#include <animals/fish/fish_simulation.hpp>
#include <animals/fish_replay/fishreplay.hpp>
#include <tools/timer.hpp>

#include <animals/fish/descriptors/cohesion_contribution_desc.hpp>
#include <animals/fish/descriptors/cohesion_desc.hpp>
#include <animals/fish/descriptors/traveled_distance_desc.hpp>
#include <animals/fish/stat/fish_in_ring_params_stat.hpp>
#include <animals/fish/stat/group_params_stat.hpp>
#include <animals/fish/stat/group_stat.hpp>
#include <animals/fish/stat/heading_stat.hpp>
#include <animals/fish/stat/polarity_stat.hpp>
#include <animals/fish/stat/position_stat.hpp>

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <sstream>

using Vec2di = std::vector<std::vector<int>>;

int to_discrete(double val, int upper_lim, int lower_lim = 0)
{
    return static_cast<int>(std::floor(val * (upper_lim - lower_lim) + lower_lim));
}

std::vector<int> split_cols(const std::string& line)
{
    std::vector<int> values;
    std::istringstream iss(line);
    int val;
    while (iss >> val)
        values.push_back(val);
    values.erase(values.begin()); // remove iteration column
    return values;
}

Vec2di read_positions(const std::string& filename)
{
    std::ifstream ifs;
    ifs.open(filename);

    std::string line;
    std::getline(ifs, line); // skip first line containing info

    Vec2di positions;
    while (std::getline(ifs, line)) {
        std::vector<int> row = split_cols(line);
        if (positions.empty())
            positions.resize(row.size());
        for (size_t i = 0; i < row.size(); ++i)
            positions[i].push_back(row[i]);
    }
    ifs.close();
    return positions;
}

int main(int argc, char** argv)
{
    assert(argc == 2);

    using namespace samsar;
    using namespace simulation;
    using namespace stat;
    using namespace desc;

    Vec2di positions = read_positions(argv[1]);

    int num_cells = 40;
    int num_robot = 1;
    int num_fish = static_cast<int>(positions.size()) - 1;
    uint64_t sim_time = positions[0].size();

    FishSimSettings set;
    set.sim_time = sim_time;
    set.stats_enabled = true;
    set.num_fish = num_fish;
    set.num_robot = num_robot;
    set.num_cells = num_cells;

    std::vector<FishIndividualPtr> fish;

    FishIndividualPtr agent = std::make_shared<FishIndividual>();
    agent->is_robot() = true;
    agent->position().x = positions[0][0];

//#define TESTING
#ifdef TESTING
    FishParams params;
    Eigen::VectorXd x(9);
    x << 1, 1, 1, 4.09828e-15, 1, 1, 6.4759e-16, 1, 1;
    params.prob_obey = x(0);
    params.prob_move = x(1);
    params.group_threshold = to_discrete(x(2), 6, 2);
    params.cells_forward = to_discrete(x(3), 8, 1);
    params.cells_backward = to_discrete(x(4), 8, 1);
    params.sum_weight
        = {(static_cast<float>(x(5)) - 0.5f) * 6, (static_cast<float>(x(6)) - 0.5f) * 6};
    params.influence_alpha = to_discrete(x(7), 8);
    params.heading_change_duration = to_discrete(x(8), num_cells / 3);

    std::cout << "Current params for the robot: " << std::endl
              << "prob_obey: " << params.prob_obey << std::endl
              << "prob_move: " << params.prob_move << std::endl
              << "group_threshold: " << params.group_threshold << std::endl
              << "cells_forward: " << params.cells_forward << std::endl
              << "cells_backward: " << params.cells_backward << std::endl
              << "sum_weight: " << params.sum_weight[0] << ", " << params.sum_weight[1] << std::endl
              << "influence_alpha: " << params.influence_alpha << std::endl
              << "heading_change_duration: " << params.heading_change_duration << std::endl;
    agent->fish_params() = params;
    agent->force_init();
#endif
    fish.push_back(agent);

    for (size_t i = 0; i < positions.size() - 1; ++i)
        fish.push_back(std::make_shared<actual::FishReplay>(positions[i]));

    FishSimulation sim(set, fish);
    sim.add_stat(std::make_shared<PositionStat>())
        .add_stat(std::make_shared<PolarityStat>())
        .add_stat(std::make_shared<PositionStat>())
        .add_stat(std::make_shared<HeadingStat>())
        .add_stat(std::make_shared<GroupStat>())
        .add_stat(std::make_shared<GroupParamsStat>())
        .add_stat(std::make_shared<FishInRingParamsStat>());

    sim.add_desc(std::make_shared<CohesionDesc>())
        .add_desc(std::make_shared<CohesionContributionDesc>(0))
        .add_desc(std::make_shared<TraveledDistanceDesc>());

    tools::Timer t;
    t.start();
    sim.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;

    auto coh = sim.descriptors()[0]->get();
    std::cout << "Cohesion: " << std::accumulate(coh.begin(), coh.end(), 0.0) / coh.size()
              << std::endl;

    return 0;
}
