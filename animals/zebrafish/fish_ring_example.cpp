#include <fish_individual.hpp>
#include <fish_simulation.hpp>
#include <tools/timer.hpp>

#include <descriptors/cohesion_contribution_desc.hpp>
#include <descriptors/cohesion_desc.hpp>
#include <descriptors/radial_interindividual_desc.hpp>
#include <descriptors/traveled_distance_desc.hpp>
#include <stat/fish_in_ring_params_stat.hpp>
#include <stat/group_params_stat.hpp>
#include <stat/group_stat.hpp>
#include <stat/heading_stat.hpp>
#include <stat/polarity_stat.hpp>
#include <stat/position_stat.hpp>

#include <Eigen/Dense>
#include <iostream>

int to_discrete(double val, int upper_lim, int lower_lim = 0)
{
    return static_cast<int>(std::floor(val * (upper_lim - lower_lim) + lower_lim));
}

int main()
{
    using namespace simu;
    using namespace simulation;
    using namespace stat;
    using namespace desc;

    int num_cells = 40;
    int num_robot = 1;
    int num_fish = 5;

    FishSimSettings set;
    set.sim_time = 7000;
    set.stats_enabled = true;
    set.num_fish = num_fish;
    set.num_robot = num_robot;
    set.num_cells = num_cells;

    std::vector<FishIndividualPtr> fish;
    for (int i = 0; i < num_fish + num_robot; ++i) {
        FishIndividualPtr def_agent = std::make_shared<FishIndividual>();
        def_agent->id() = i;
        if (i < num_robot)
            def_agent->is_robot() = true;
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

    return 0;
}
