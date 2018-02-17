#ifndef FISH_IN_RING_SIM_HPP
#define FISH_IN_RING_SIM_HPP

#include <random_generator.hpp>
#include <simulation.hpp>
#include <types.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

#include <algorithm>
#include <cstdint>
#include <math.h>
#include <set>
#include <vector>

namespace sim {
    template <typename Params, typename FishType> class FishInRing : public SimulatorBase<Params> {
    public:
        using base_type = SimulatorBase<Params>;
        FishInRing()
            : num_agents_(Params::fish_in_ring::num_fish + Params::fish_in_ring::num_robot),
              cell_degree_(360.0 / Params::fish_in_ring::num_cells),
              num_cells_look_(std::ceil(Params::fish_in_ring::deg_vision / cell_degree_))
        {
            fish_.resize(num_agents_);
            int robots_to_assign = Params::fish_in_ring::num_robot;
            for (auto& f : fish_) {
                f.position() = std::floor(tools::random_in_range(0.0f, 1.0f) * Params::fish_in_ring::num_cells);
                if (robots_to_assign-- > 0)
                    f.is_robot(true);
            }
        }

        void spin_once() override
        {
            using namespace types;
            for (size_t i = 0; i < num_agents_; ++i) {
                // calculate the neighborhoods according to the agent's heading
                // and the problem parameters:
                // degrees of vision + cell degrees = number of cells to look
                std::vector<int> neighborhood;
                boost::push_back(neighborhood,
                    boost::irange(fish_[i].position(),
                        fish_[i].position() + fish_[i].heading() * static_cast<int>(num_cells_look_),
                        fish_[i].heading()));
                std::for_each(neighborhood.begin(), neighborhood.end(),
                    [](int& v) { v %= static_cast<int>(Params::fish_in_ring::num_cells); });

                // iterate through all the cells the fish can look for neighbors
                // until the max neighbor criteria is met or the fish runs out of cells
                size_t num_neighbors = 0;
                int sum_heading = Heading::UNDEFINED;
                for (size_t j = 0; j < num_cells_look_; ++j) {
                    // find the fish that belong to the jth neighborhood
                    std::vector<size_t> neighbors = find_neighbors(neighborhood.at(j), i);
                    if (neighbors.size()) {
                        // pick the neighbors in a random way
                        // TODO: we should insert more criteria for the fish selection
                        std::shuffle(neighbors.begin(), neighbors.end(), tools::random_device_);
                        for (size_t k = 0; k < neighbors.size();
                             ++num_neighbors, sum_heading += fish_[neighbors.at(k)].heading(), ++k)
                            if (num_neighbors == Params::fish_in_ring::max_neighbors)
                                break;
                    }
                    if (num_neighbors == Params::fish_in_ring::max_neighbors)
                        break;
                } // for j (num_cells_look_)

                // Update the headings - No probabilistic decisions at this point
                // -- enforce the heading of the robots
                if (fish_[i].is_robot()) {
                    fish_[i].heading() = Params::fish_in_ring::heading_robot;
                    continue;
                }

                // -- calculate the new heading using both the fish's heading
                // -- and the collective heading (sum of headings)
                Heading new_heading = to_heading(fish_[i].heading() + sum_heading);
                if (new_heading == Heading::UNDEFINED)
                    // if the collective heading is balanced between clockwise and
                    // counter-clockwise, then we assume the fish will keep its heading
                    new_heading = fish_[i].heading();
                fish_[i].heading() = new_heading;
            } // for i (num_agents_)

            // Update the headings & positions - Add some probabilistic logic
            // -- some fish are likely to disobey the shoal and reverse its heading
            std::for_each(fish_.begin(), fish_.end(), [](FishType& f) {
                if (!f.is_robot()) {
                    bool obey = tools::random_in_range(0.0f, 1.0f) < Params::fish_in_ring::prob_obedience;
                    if (!obey)
                        f.heading() = reverse_heading(f.heading());
                }
                // for now robotic fish retain their heading

                bool move;
                (f.is_robot()) ? (move = tools::random_in_range(0.0f, 1.0f) < Params::fish_in_ring::prob_move)
                               : (move = tools::random_in_range(0.0f, 1.0f) < (1 - Params::fish_in_ring::prob_stay));
                if (move)
                    f.position() = (f.position() + f.heading()) % static_cast<int>(Params::fish_in_ring::num_cells);
            });
        }

    private:
        std::vector<size_t> find_neighbors(int neighborhood, size_t self) const
        {
            std::vector<size_t> neighbors;
            for (size_t i = 0; i < fish_.size(); ++i) {
                if (i == self)
                    continue;
                if (fish_[i].position() == neighborhood)
                    neighbors.push_back(i);
            }
            return neighbors;
        }

        const size_t num_agents_;
        const float cell_degree_;
        const size_t num_cells_look_;
        std::vector<FishType> fish_;
    };
} // namespace sim

#endif
