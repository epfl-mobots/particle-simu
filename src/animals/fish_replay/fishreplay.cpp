#include "fishreplay.hpp"

#include <animals/fish/fish_simulation.hpp>
#include <animals/fish/heading.hpp>
#include <tools/math.hpp>

namespace samsar {
    namespace actual {
        using namespace types;

        FishReplay::FishReplay(const std::vector<int>& trajectory)
            : FishIndividual(), _trajectory(trajectory)
        {
            _init();
        }

        FishReplay::~FishReplay() {}

        void FishReplay::_init()
        {
            _position.x = _trajectory[0]; // initialize position
            _next_heading = Heading::UNDEFINED;
            _heading = Heading::UNDEFINED;
        }

        void FishReplay::force_init()
        {
            FishIndividual::force_init();
            _init();
        }

        void FishReplay::stimulate(const std::shared_ptr<simulation::Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<simulation::FishSimulation>(sim);
            std::vector<simulation::FishIndividualPtr> fish = fsim->fish();
            int num_cells = fsim->fish_sim_settings().num_cells;

            if (sim->iteration() < _trajectory.size() - 1) {
                int dist = _trajectory[sim->iteration() + 1] - _trajectory[sim->iteration()];
                _next_heading = to_heading(dist);
                if (std::abs(dist) > static_cast<int>(num_cells / 2)) {
                    dist = std::abs(static_cast<int>(num_cells) - dist);
                    _next_heading = reverse_heading(_next_heading);
                }
            }

            if (sim->iteration() > 0)
                _speed.set_speed_bounded(
                    std::abs(_trajectory[sim->iteration()] - _trajectory[sim->iteration() - 1]));
        }

        void FishReplay::move(const std::shared_ptr<simulation::Simulation> sim)
        {
            Heading save_heading = _next_heading;
            int save_pos = _trajectory[sim->iteration()];

            _heading = save_heading;
            if (_heading == Heading::UNDEFINED)
                _heading = random_heading();

            FishIndividual::stimulate(sim);
            FishIndividual::move(sim);

            _predicted_hdg.push_back(_heading == save_heading);
            _predicted_move.push_back(_position.x == save_pos);

            _heading = save_heading;
            _position.x = save_pos;
        }

    } // namespace actual
} // namespace samsar
