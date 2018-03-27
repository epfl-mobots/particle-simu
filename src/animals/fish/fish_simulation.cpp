#include "fish_simulation.hpp"

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace samsar {
    namespace simulation {

        FishSimulation::FishSimulation(bool stats_enable) : Simulation(stats_enable)
        {
            _sim_settings.add_setting("num_fish", 6)
                .add_setting("num_robot", 0)
                .add_setting("num_cells", 40)
                .add_setting("sim_time", 5400);
            // _sim_settings.add_setting("deg_vision", 120); // not used anymore
            _init();
        }

        FishSimulation::FishSimulation(const Settings& settings)
            : Simulation(settings.get_field<bool>("stats_enabled")->value())
        {
            _sim_settings = settings;
            _init();
        }

        FishSimulation::FishSimulation(
            const Settings& settings, const std::vector<FishIndividual>& fish)
            : Simulation(settings.get_field<bool>("stats_enabled")->value()), _fish(fish)
        {
            size_t count = 0;
            for (const auto& f : _fish)
                if (f.is_robot())
                    ++count;
            _sim_settings = settings;
            _sim_settings.add_setting("num_agents", _fish.size() + count);
            _sim_settings.add_setting("num_robot", count);
            _sim_settings.add_setting("num_fish", _fish.size() - count);
        }

        void FishSimulation::_init()
        {
            int num_agents = _sim_settings.get_field<int>("num_fish")->value()
                + _sim_settings.get_field<int>("num_robot")->value();
            _sim_settings.add_setting("num_agents", num_agents);

            _fish.resize(static_cast<size_t>(num_agents));
            for (size_t i = 0; i < _fish.size(); ++i)
                _fish[i].id() = static_cast<int>(i);
            for (size_t i = 0;
                 i < static_cast<size_t>(_sim_settings.get_field<int>("num_robot")->value()); ++i)
                _fish[i].is_robot() = true;
        }

        void FishSimulation::spin_once()
        {
            using namespace types;

            std::vector<int> idcs;
            boost::push_back(idcs, boost::irange(0, static_cast<int>(_fish.size()), 1));
            std::shuffle(idcs.begin(), idcs.end(), tools::RandomGenerator().gen());

            // stimulate the fish to drive their movement decisions
            for (const int idx : idcs)
                _fish[static_cast<size_t>(idx)].stimulate(std::make_shared<FishSimulation>(*this));

            // apply intuitions and move towards the respective heading
            // the fish might discard this intuition and stay in its
            // position or even move towards the oposite heading of the shoal
            for (const int idx : idcs)
                _fish[static_cast<size_t>(idx)].move(std::make_shared<FishSimulation>(*this));

            // update statistics
            _update_stats(std::make_shared<FishSimulation>(*this));

            Simulation::spin_once();
        }

        std::vector<FishIndividual> FishSimulation::fish() const { return _fish; }
        std::vector<FishIndividual>& FishSimulation::fish() { return _fish; }

    } // namespace simulation
} // namespace samsar
