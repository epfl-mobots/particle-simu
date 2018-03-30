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
            _position.x = _trajectory[0]; // initialize position
            _next_heading = Heading::UNDEFINED;
            _heading = Heading::UNDEFINED;
        }

        FishReplay::~FishReplay() {}

        void FishReplay::stimulate(const std::shared_ptr<simulation::Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<simulation::FishSimulation>(sim);
            std::vector<simulation::FishIndividualPtr> fish = fsim->fish();
            int num_cells = fsim->sim_settings().get_field<int>("num_cells")->value();

            if (sim->iteration() < _trajectory.size() - 1) {
                int diff = _trajectory[sim->iteration() + 1] - _trajectory[sim->iteration()];
                _next_heading = to_heading(diff);
                if ((diff <= -(num_cells / 2)) || (diff > num_cells / 2))
                    _next_heading = reverse_heading(_next_heading);
            }

            if (sim->iteration() > 0)
                _speed.set_speed_bounded(
                    std::abs(_trajectory[sim->iteration()] - _trajectory[sim->iteration() - 1]));

            if (_next_heading == Heading::UNDEFINED)
                _next_heading = random_heading();
        }

        void FishReplay::move(const std::shared_ptr<simulation::Simulation> sim)
        {
            _heading = _next_heading;
            _position.x = _trajectory[sim->iteration()];
        }

    } // namespace actual
} // namespace samsar
