#ifndef COHESION_DESC_HPP
#define COHESION_DESC_HPP

#include <fish_simulation.hpp>
#include <descriptors/descriptor_base.hpp>

#include <complex>

namespace simu {
    namespace desc {
        using namespace simulation;

        class CohesionDesc : public DescriptorBase {
        public:
            CohesionDesc() {}

            virtual void operator()(const std::shared_ptr<Simulation> sim) override
            {
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);
                std::vector<int> positions;
                for (size_t i = 0; i < fsim->fish().size(); ++i)
                    positions.push_back(fsim->fish()[i]->position().x);
                _desc.push_back(_compute_cohesion(
                    positions, positions.size(), fsim->fish_sim_settings().num_cells));
            }

        protected:
            double _compute_cohesion(std::vector<int> positions, int N, int M)
            {
                std::complex<double> cohexp(0, 0);
                for (size_t i = 0; i < positions.size(); ++i)
                    cohexp += std::exp(std::complex<double>(0, 2 * M_PI * positions[i] / M));
                return std::abs(cohexp) / N;
            }
        };

    } // namespace desc
} // namespace simu

#endif
