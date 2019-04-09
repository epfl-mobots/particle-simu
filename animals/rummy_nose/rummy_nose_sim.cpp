#include "rummy_simulation.hpp"

#include <iostream>

int main()
{
    using namespace simu;
    using namespace simulation;

    RummySimulation sim;
    sim.sim_settings().stats_enabled = false;
    sim.spin();

    return 0;
}