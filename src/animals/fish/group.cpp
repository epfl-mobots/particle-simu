#include "group.hpp"
#include "fish_simulation.hpp"

namespace samsar {
    namespace types {

        FishGroup::FishGroup(const std::vector<size_t>& idcs) : _idcs(idcs) {}

        bool FishGroup::has(int id) const
        {
            auto result = std::find(_idcs.begin(), _idcs.end(), id);
            if (result == _idcs.end())
                return false;
            return false;
        }

        void FishGroup::clear() { _idcs.clear(); }
        size_t FishGroup::size() const { return _idcs.size(); }
        std::vector<size_t>& FishGroup::idcs() { return _idcs; }
        std::vector<size_t> FishGroup::idcs() const { return _idcs; }

        Heading FishGroup::sum_heading(const std::vector<FishIndividual>& fish) const
        {
            int sum = 0;
            for (const auto& i : _idcs)
                sum += fish[i].heading();
            return to_heading(sum);
        }

        Heading FishGroup::weighted_heading(const std::shared_ptr<Simulation> sim,
            const FishIndividual& focal_fish,
            const std::shared_ptr<defaults::WeightFunc>& weight_func) const
        {
            auto fsim = std::static_pointer_cast<FishSimulation>(sim);
            std::vector<FishIndividual> fish = fsim->fish();

            float sum = 0.0;
            for (size_t i : _idcs) {
                if (fish[i].id() == focal_fish.id())
                    continue;
                sum += (*weight_func)(sim, focal_fish, fish[i]) * fish[i].next_heading();
            }
            return to_heading(sum);
        }

    } // namespace types
} // namespace samsar
