#include <animals/fish/fish_simulation.hpp>
#include <animals/fish_replay/fishreplay.hpp>
#include <tools/timer.hpp>

#include <animals/fish/descriptors/cohesion_contribution_desc.hpp>
#include <animals/fish/descriptors/cohesion_desc.hpp>
#include <animals/fish/descriptors/radial_interindividual_desc.hpp>
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
    //    int num_robot = 1;
    //    int num_fish = static_cast<int>(positions.size()) - 1;
    int num_robot = 0;
    int num_fish = static_cast<int>(positions.size());
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
    agent->id() = 0;
    agent->position().x = positions[0][0];

//#define TESTING
#ifdef TESTING
    // num_cells: 159
    // prob_obey: 1
    // prob_move: 1
    // prob_change_speed: 0
    // group_threshold: 3
    // cells_forward: 23
    // cells_backward: 23
    // min_speed: 1
    // max_speed: 1
    // sum_weight: 1 -1
    // heading_change_duration: 53
    // influence_alpha: 1
    // heading_bias: 0
    // Average radial distance values: 0.882757 0.861899 0.869066 0.875058 0.872716 0.875833
    // 0.905551 0.881545 0.913723 0.861875 (38.1593) Average hdg perc values: 0.966383 0.970332
    // 0.96109 0.970302 0.960264 0.967174 0.962488 0.966522 0.962263 0.967551
    //>>> Fitness: 0.880002 0.965437

    set.num_cells = 159;
    FishParams params;
    //    Eigen::VectorXd x(9);
    //    x << 1, 1, 1, 4.09828e-15, 1, 1, 6.4759e-16, 1, 1;
    params.prob_obey = 1.0f;
    params.prob_move = 1.0f;
    params.prob_change_speed = 0;
    params.group_threshold = 3;
    params.cells_forward = 23;
    params.cells_backward = 23;
    params.sum_weight = {1, -1};
    params.heading_change_duration = 53;
    params.influence_alpha = 1;
    params.min_speed = 1;
    params.max_speed = 1;

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
    //    fish.push_back(agent);

    for (int i = num_robot; i < num_fish; ++i) {
        auto agent = std::make_shared<actual::FishReplay>(positions[i]);
#ifdef TESTING
        agent->fish_params() = params;
        agent->force_init();
#endif
        agent->id() = i + num_robot;
        fish.push_back(agent);
    }

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
        .add_desc(std::make_shared<TraveledDistanceDesc>())
        .add_desc(std::make_shared<RadialInterindividualDesc>());

    tools::Timer t;
    t.start();
    sim.spin();
    double elapsed_time = t.stop();
    std::cout << "Simulation ended after " << elapsed_time << "ms" << std::endl;

    auto coh = sim.descriptors()[0]->get();
    std::cout << "Cohesion: " << std::accumulate(coh.begin(), coh.end(), 0.0) / coh.size()
              << std::endl;
    auto dist = sim.descriptors()[3]->get();
    std::cout << "Average distance to centroid: "
              << std::accumulate(dist.begin(), dist.end(), 0.0) / dist.size() << std::endl;

    std::vector<float> hdgs;
    std::vector<float> moves;
    for (size_t i = num_robot; i < sim.fish().size(); ++i) { // skip artificial fish
        float hdg_success = 0;
        float pos_success = 0;
        auto focal_fish = reinterpret_cast<actual::FishReplay*>(sim.fish()[i].get());

        std::for_each(focal_fish->predicted_hdg().begin(), focal_fish->predicted_hdg().end(),
            [&hdg_success](bool s) { hdg_success += s; });
        hdg_success /= focal_fish->predicted_hdg().size();
        hdgs.push_back(hdg_success);

        std::for_each(focal_fish->predicted_move().begin(), focal_fish->predicted_move().end(),
            [&pos_success](bool s) { pos_success += s; });
        pos_success /= focal_fish->predicted_move().size();
        moves.push_back(pos_success);
    }

    std::for_each(hdgs.begin(), hdgs.end(), [](float perc) { std::cout << perc << " "; });
    std::cout << std::endl;
    std::for_each(moves.begin(), moves.end(), [](float perc) { std::cout << perc << " "; });
    std::cout << std::endl;

    return 0;
}
