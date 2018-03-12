#ifndef GROUP_HPP
#define GROUP_HPP

#include <algorithm>
#include <map>
#include <types/movement/heading.hpp>
#include <vector>

namespace samsar {
    namespace types {

        struct InvertedFishTable {
            template <typename FishType>
            std::map<int, std::vector<FishType>> operator()(const std::vector<FishType>& fish)
            {
                std::map<int, std::vector<FishType>> ipos;
                for (const FishType& f : fish)
                    ipos[f.position()].push_back(f);
                return ipos;
            }
        };

        template <typename FishType> struct Group {
        public:
            Group() : group_id_(-1), center_of_mass_(-1) {}

            Group(const std::vector<FishType>& fish) : group_id_(-1), center_of_mass_(-1), fish_(fish) { calc_cog(); }

            bool has(int id) const
            {
                for (const auto& f : fish_)
                    if (f.id() == id)
                        return true;
                return false;
            }

            Heading heading() const
            {
                int sum = 0;
                for (const auto& f : fish_)
                    sum += f.heading();
                return to_heading(sum);
            }

            Heading imposed_heading() const
            {
                std::vector<FishType> sorted_fish = fish_;
                std::sort(sorted_fish.begin(), sorted_fish.end(),
                    [](const FishType& lhs, const FishType& rhs) { return lhs.prob_lead() > rhs.prob_lead(); });
                return sorted_fish.at(0).heading();
            }

            size_t size() const { return fish_.size(); }
            int group_id() const { return group_id_; }
            int& group_id() { return group_id_; }
            int center_of_mass() const { return center_of_mass_; }
            int& center_of_mass() { return center_of_mass_; }
            std::vector<FishType>& fish() { return fish_; }
            std::vector<FishType> fish() const { return fish_; }

        private:
            void calc_cog()
            {
                auto ift = InvertedFishTable()(this->fish_);
                std::vector<std::pair<int, int>> pairs;
                for (const auto& p : ift)
                    pairs.push_back(std::make_pair(p.first, p.second.size()));
                std::sort(
                    pairs.begin(), pairs.end(), [](const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
                        return lhs.second > rhs.second;
                    });
                center_of_mass_ = pairs.at(0).first;
            }

            int group_id_;
            int center_of_mass_;
            std::vector<FishType> fish_;
        };

    } // namespace types
} // namespace samsar

#endif
