#ifndef FISH_IN_RING_SIM_HPP
#define FISH_IN_RING_SIM_HPP

#include <random/random_generator.hpp>
#include <simulation/simulation_base.hpp>
#include <types/types.hpp>

#include <algorithm>
#include <cstdint>
#include <math.h>
#include <set>
#include <vector>

namespace samsar {
    namespace sim {
        template <typename Params, typename FishType, typename Stat = stat::StatBase<Params>>
        class FishInRing : public SimulationBase<Params, Stat> {
        public:
            FishInRing()
                : num_agents_(Params::fish_in_ring::num_fish + Params::fish_in_ring::num_robot),
                  cell_degree_(360.0 / Params::fish_in_ring::num_cells),
                  num_cells_look_(std::ceil(Params::fish_in_ring::deg_vision / cell_degree_))
            {
                fish_.resize(num_agents_);
                int robots_to_assign = Params::fish_in_ring::num_robot;
                for (size_t i = 0; i < fish_.size(); ++i) {
                    fish_[i].position()
                        = std::floor(tools::random_in_range(0.0f, 1.0f) * Params::fish_in_ring::num_cells);
                    if (robots_to_assign-- > 0)
                        fish_[i].is_robot(true);
                    fish_[i].set_id(i);
                }
            }

            void spin_once() override
            {
                using namespace types;

                // update statistics
                this->update_stats(this);

                // calculate fish intuitions in accordance with the shoal status
                for (const auto& f : fish_)
                    f.calc_intuitions(fish_, num_cells_look_);

                // apply intuitions and move towards the respective heading
                // the fish might discard this intuition and stay in its
                // position or even move towards the oposite heading of the shoal
                for (auto& f : fish_)
                    f.move();
            }

            const std::vector<FishType>& fish() const { return fish_; }
            size_t num_agents() const { return num_agents_; }

        private:
            const size_t num_agents_;
            const float cell_degree_;
            const size_t num_cells_look_;
            std::vector<FishType> fish_;
        };
    } // namespace sim
} // namespace samsar

#endif
