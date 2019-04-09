#ifndef COHESION_CONTRIBUTION_DESC_HPP
#define COHESION_CONTRIBUTION_DESC_HPP

#include <descriptors/cohesion_desc.hpp>
#include <fish_simulation.hpp>

#include <complex>

namespace simu {
    namespace desc {
        using namespace simulation;

        class CohesionContributionDesc : public CohesionDesc {
        public:
            CohesionContributionDesc(int individual_index) : _individual_idx(individual_index) {}

            virtual void operator()(const std::shared_ptr<Simulation> sim) override
            {
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);
                std::vector<int> positions;
                for (size_t i = 0; i < fsim->fish().size(); ++i)
                    positions.push_back(fsim->fish()[i]->position().x);

                double coh_all = _compute_cohesion(
                    positions, positions.size(), fsim->fish_sim_settings().num_cells);
                positions.erase(positions.begin() + _individual_idx);
                double coh_partial = _compute_cohesion(
                    positions, positions.size(), fsim->fish_sim_settings().num_cells);
                _desc.push_back(coh_all - coh_partial);
            }

            int individual_index() const { return _individual_idx; }

        protected:
            int _individual_idx;
        };

    } // namespace desc
} // namespace simu

#endif
