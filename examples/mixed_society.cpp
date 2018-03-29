#include <fstream>
#include <iostream>
#include <sstream>

#include <animals/fish/fish_simulation.hpp>
#include <animals/fish_replay/fishreplay.hpp>
#include <tools/timer.hpp>

#include <animals/fish/stat/fish_in_ring_params_stat.hpp>
#include <animals/fish/stat/group_params_stat.hpp>
#include <animals/fish/stat/group_stat.hpp>
#include <animals/fish/stat/heading_stat.hpp>
#include <animals/fish/stat/polarity_stat.hpp>
#include <animals/fish/stat/position_stat.hpp>

using Vec2di = std::vector<std::vector<int>>;

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

    Vec2di positions = read_positions(argv[1]);

    int num_cells = 40;
    int num_robot = 1;
    int num_fish = static_cast<int>(positions.size()) - 1;
    int sim_time = static_cast<int>(positions[0].size());

    Settings sim_set;
    sim_set.add_setting("stats_enabled", true)
        .add_setting("num_fish", num_fish)
        .add_setting("num_robot", num_robot)
        .add_setting("num_cells", num_cells)
        .add_setting("sim_time", sim_time);

    std::vector<FishIndividualPtr> fish;

    FishIndividualPtr agent = std::make_shared<FishIndividual>();
    agent->is_robot() = true;
    agent->position().x = positions[0][0];
    fish.push_back(agent);
    for (size_t i = 0; i < positions.size(); ++i)
        fish.push_back(std::make_shared<actual::FishReplay>(positions[i]));

    FishSimulation sim(sim_set, fish);
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
