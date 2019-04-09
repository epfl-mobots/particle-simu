#ifndef TRAVELED_DISTANCE_DESC_HPP
#define TRAVELED_DISTANCE_DESC_HPP

#include <fish_simulation.hpp>
#include <descriptors/descriptor_base.hpp>

#include <complex>

namespace simu {
    namespace desc {
        using namespace simulation;

        class TraveledDistanceDesc : public DescriptorBase {
        public:
            TraveledDistanceDesc() {}

            virtual void operator()(const std::shared_ptr<Simulation> sim) override
            {
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);
                if (_desc.size() == 0) {
                    _desc.resize(fsim->fish().size(), 0.0);
                    for (size_t i = 0; i < fsim->fish().size(); ++i)
                        _prev_pos.push_back(fsim->fish()[i]->position().x);
                } // skip computations for the first iteration -> no distance traveled
                else {
                    int num_cells = fsim->fish_sim_settings().num_cells;
                    for (size_t i = 0; i < fsim->fish().size(); ++i) {
                        int current_pos = fsim->fish()[i]->position().x;
                        int dist = current_pos - _prev_pos[i];
                        if ((dist <= -num_cells / 2) || (dist > num_cells / 2))
                            dist = sgn(dist) * (num_cells - std::abs(dist));
                        _desc[i] += dist;
                        _prev_pos[i] = current_pos;
                    }
                }
            }

            template <typename T>
            int sgn(T val) const { return (T(0) < val) - (val < T(0)); }

        protected:
            std::vector<int> _prev_pos;
        };

    } // namespace desc
} // namespace simu

#endif
