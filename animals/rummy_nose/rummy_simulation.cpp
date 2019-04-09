#include "rummy_simulation.hpp"

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace simu {
    namespace simulation {

        RummySimulation::RummySimulation(bool stats_enable) : Simulation(stats_enable) { _init(); }

        void RummySimulation::_init()
        {
            // _sim_settings.sim_time = _fish_sim_settings.sim_time;
            // _sim_settings.stats_enabled = _fish_sim_settings.stats_enabled;

            // int num_agents = _fish_sim_settings.num_fish + _fish_sim_settings.num_robot;
            // _fish_sim_settings.num_agents = num_agents;

            // _fish.resize(static_cast<size_t>(num_agents));
            // for (size_t i = 0; i < _fish.size(); ++i) {
            //     _fish[i] = std::make_shared<FishIndividual>();
            //     _fish[i]->id() = static_cast<int>(i);
            // }
            // for (size_t i = 0; i < static_cast<size_t>(_fish_sim_settings.num_robot); ++i)
            //     _fish[i]->is_robot() = true;
        }

        void RummySimulation::spin_once()
        {
            //     using namespace types;

            //     std::vector<int> idcs;
            //     boost::push_back(idcs, boost::irange(0, static_cast<int>(_fish.size()), 1));
            //     std::shuffle(idcs.begin(), idcs.end(), tools::RandomGenerator().gen());

            //     // stimulate the fish to drive their movement decisions
            //     for (const int idx : idcs)
            //         _fish[static_cast<size_t>(idx)]->stimulate(std::make_shared<RummySimulation>(*this));

            //     // apply intuitions and move towards the respective heading
            //     // the fish might discard this intuition and stay in its
            //     // position or even move towards the oposite heading of the shoal
            //     for (const int idx : idcs)
            //         _fish[static_cast<size_t>(idx)]->move(std::make_shared<RummySimulation>(*this));

            //     // update statistics
            //     _update_stats(std::make_shared<RummySimulation>(*this));
            //     _update_descriptors(std::make_shared<RummySimulation>(*this));

            std::cout << _iteration << std::endl;
            Simulation::spin_once();
        }

        // std::vector<FishIndividualPtr> RummySimulation::fish() const { return _fish; }
        // std::vector<FishIndividualPtr>& RummySimulation::fish() { return _fish; }

        // FishSimSettings RummySimulation::fish_sim_settings() const { return _fish_sim_settings; }
        // FishSimSettings& RummySimulation::fish_sim_settings() { return _fish_sim_settings; }

    } // namespace simulation
} // namespace simu
