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
    namespace defaults {
        struct ring {
            static constexpr size_t num_fish = 5;
            static constexpr size_t num_robot = 1;
            static constexpr size_t num_cells = 40;
            static constexpr int deg_vision = 120;
        };
    } // namespace defaults

    namespace sim {
        template <typename Params, typename FishType, typename Stat = stat::StatBase<Params>>
        class FishInRing : public SimulationBase<Params, Stat> {

            using base_type_t = SimulationBase<Params, Stat>;

        public:
            FishInRing()
                : num_agents_(Params::ring::num_fish + Params::ring::num_robot),
                  num_cells_(Params::ring::num_cells),
                  cell_degree_(360.0 / Params::ring::num_cells),
                  num_cells_look_(std::ceil(Params::ring::deg_vision / cell_degree_))
            {
                int robots_to_assign = Params::ring::num_robot;
                for (size_t i = 0; i < num_agents_; ++i) {
                    fish_.push_back(FishType(num_cells_));
                    fish_[i].position() = std::floor(tools::random_in_range(0.0f, 1.0f) * Params::ring::num_cells);
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
                for (auto& f : fish_)
                    f.calc_intuitions(fish_, num_cells_look_);

                // apply intuitions and move towards the respective heading
                // the fish might discard this intuition and stay in its
                // position or even move towards the oposite heading of the shoal
                for (auto& f : fish_)
                    f.move();

                base_type_t::spin_once();
            }

            const std::vector<FishType>& fish() const { return fish_; }
            std::vector<FishType>& fish() { return fish_; }
            size_t num_agents() const { return num_agents_; }

        protected:
            const size_t num_agents_;
            const size_t num_cells_;
            const float cell_degree_;
            const size_t num_cells_look_;
            std::vector<FishType> fish_;
        };
    } // namespace sim
} // namespace samsar

#endif
