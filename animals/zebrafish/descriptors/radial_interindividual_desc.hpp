#ifndef RADIAL_INTERINDIVIDUAL_DESC_HPP
#define RADIAL_INTERINDIVIDUAL_DESC_HPP

#include <fish_simulation.hpp>
#include <descriptors/descriptor_base.hpp>

#include <complex>

namespace simu {
    namespace desc {
        using namespace simulation;

        class RadialInterindividualDesc : public DescriptorBase {
        public:
            explicit RadialInterindividualDesc() {}

            virtual void operator()(const std::shared_ptr<Simulation> sim) override
            {
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);
                _num_cells = fsim->fish_sim_settings().num_cells;
                _desc.push_back(_compute_centroid(fsim->fish()));
            }

        protected:
            template <typename Position>
            int _distance(const Position& ind1, const Position& ind2, int num_cells) const
            {
                int dist = ind1.x - ind2.x;
                if (std::abs(dist) > static_cast<int>(num_cells / 2))
                    dist = std::abs(static_cast<int>(num_cells) - dist);
                return std::abs(dist);
            }

            double _compute_centroid(const std::vector<FishIndividualPtr>& fish) const
            {
                double fish_centroid = 0;
                for (size_t i = 0; i < fish.size(); ++i) {
                    double focal_distance = 0;
                    for (size_t j = 0; j < fish.size(); ++j) {
                        if (i == j)
                            continue;

                        focal_distance
                            += _distance(fish[i]->position(), fish[j]->position(), _num_cells);
                    }
                    fish_centroid += focal_distance / (fish.size() - 1);
                }
                fish_centroid /= fish.size();
                return fish_centroid;
            }

            int _num_cells;
        };

    } // namespace desc
} // namespace simu

#endif
