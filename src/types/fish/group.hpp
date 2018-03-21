#ifndef GROUP_HPP
#define GROUP_HPP

#include <algorithm>
#include <map>
#include <types/movement/heading.hpp>
#include <vector>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

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

        namespace defaults {
            struct WeightFunc {
                WeightFunc() : alpha_f_(0.3f), alpha_b_(-2) {}

                template <typename Fish>
                float operator()(const std::vector<Fish>& /*fv*/, const Fish& ff, const Fish& f) const
                {
                    std::vector<int> forward;
                    boost::push_back(forward,
                        boost::irange(ff.position() + ff.heading() * static_cast<int>(ff.cells_forward()),
                            ff.position() - ff.heading(), -ff.heading()));
                    std::vector<int> backward;
                    boost::push_back(backward,
                        boost::irange(ff.position() - ff.heading(),
                            ff.position() + (-ff.heading() * static_cast<int>(ff.cells_backward())) - ff.heading(),
                            -ff.heading()));
                    std::for_each(forward.begin(), forward.end(),
                        [&](int& v) { (v < 0) ? v += ff.num_cells() : v %= static_cast<int>(ff.num_cells()); });
                    std::for_each(backward.begin(), backward.end(),
                        [&](int& v) { (v < 0) ? v += ff.num_cells() : v %= static_cast<int>(ff.num_cells()); });

                    const auto itf = std::find(forward.begin(), forward.end(), f.position());
                    if (!(forward.end() == itf)) {
                        auto idx = std::distance(forward.begin(), itf);
                        return std::exp(idx * alpha_f_);
                    }

                    const auto itb = std::find(backward.begin(), backward.end(), f.position());
                    if (!(backward.end() == itb)) {
                        auto idx = std::distance(backward.begin(), itb);
                        return std::exp(alpha_b_ * idx);
                    }
                    return 0;
                }

                float alpha_f_;
                float alpha_b_;
            };
        } // namespace defaults

        template <typename FishType, typename WeightFunc = defaults::WeightFunc> struct Group {
        public:
            Group() : group_id_(-1), center_of_mass_(-1) {}
            Group(const std::vector<FishType>& fish) : group_id_(-1), center_of_mass_(-1), fish_(fish) { calc_com(); }

            void set(const std::vector<FishType>& fish)
            {
                fish_ = fish;
                calc_com();
            }

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

            Heading weighted_heading(const FishType& focal_fish) const
            {
                float sum = 0.0;
                for (const auto& f : fish_) {
                    if (focal_fish.id() == f.id())
                        continue;
                    sum += weight_func_(fish_, focal_fish, f) * f.next_heading();
                }
                return to_heading(sum);
            }

            Heading imposed_heading() const
            {
                std::vector<FishType> sorted_fish = fish_;
                std::sort(sorted_fish.begin(), sorted_fish.end(),
                    [](const FishType& lhs, const FishType& rhs) { return lhs.prob_lead() > rhs.prob_lead(); });
                return sorted_fish.at(0).heading();
            }

            void clear()
            {
                fish_.clear();
                group_id_ = -1;
                center_of_mass_ = -1;
            }

            size_t size() const { return fish_.size(); }

            int group_id() const { return group_id_; }
            int& group_id() { return group_id_; }
            int center_of_mass() const { return center_of_mass_; }
            int& center_of_mass() { return center_of_mass_; }
            WeightFunc& weight_func() { return weight_func_; }
            std::vector<FishType>& fish() { return fish_; }
            std::vector<FishType> fish() const { return fish_; }

        private:
            void calc_com()
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
            WeightFunc weight_func_;
        };

    } // namespace types
} // namespace samsar

#endif
