#ifndef GROUP_ZEBRAFISH_HPP
#define GROUP_ZEBRAFISH_HPP

#include <fish/zebrafish.hpp>

#include <map>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace samsar {
    namespace defaults {
        struct group_zebrafish {
            static constexpr size_t group_threshold = 2;
            static constexpr size_t group_cells_forward = 3;
            static constexpr size_t group_cells_backward = 3;
            static constexpr float social_influence = 0.9f;
        };
    } // namespace defaults

    namespace fish {
        template <typename Params> class GroupZebrafish : public Zebrafish<Params> {

            using base_type_t = Zebrafish<Params>;
            using InvertedFishTable = std::map<int, std::vector<GroupZebrafish>>;

        public:
            GroupZebrafish()
                : in_group_(false),
                  group_threshold_(Params::group_zebrafish::group_threshold),
                  group_cells_forward_(Params::group_zebrafish::group_cells_forward),
                  group_cells_backward_(Params::group_zebrafish::group_cells_backward),
                  social_influence_(Params::group_zebrafish::social_influence)
            {
            }

            GroupZebrafish shallow_clone() const
            {
                GroupZebrafish cpy;
                cpy.id_ = this->id_;
                cpy.heading_ = this->heading_;
                cpy.next_heading_ = this->next_heading_;
                cpy.position_ = this->position_;
                cpy.is_robot_ = this->is_robot_;
                return cpy;
            }

            void move() override
            {
                if (this->heading() == Heading::UNDEFINED)
                    return;

                if (!in_group_)
                    base_type_t::move();
                else {
                    this->heading_ = this->next_heading_;

                    if (!this->is_robot()) {
                        bool is_influenced = tools::random_in_range(0.0f, 1.0f) < social_influence_;
                        if (!is_influenced)
                            this->heading() = reverse_heading(this->heading());
                        else {
                            auto result = group_sort_positions();
                            this->heading_ = to_heading(result[0].first - this->position_);
                            if (this->heading_ == Heading::UNDEFINED)
                                this->heading_ = this->next_heading_;
                        }
                    }
                    else
                        this->heading_ = this->heading_robot_;

                    bool move;
                    (this->is_robot()) ? (move = tools::random_in_range(0.0f, 1.0f) < this->prob_move_)
                                       : (move = tools::random_in_range(0.0f, 1.0f) < (1 - this->prob_stay_));
                    if (move) {
                        this->position() = (this->position() + this->heading()) % static_cast<int>(this->num_cells_);
                        if (this->position_ < 0)
                            this->position_ += this->num_cells_;
                    }
                }
            }

            template <typename Shoal> void calc_intuitions(const Shoal& shoal, size_t num_cells_look)
            {
                base_type_t::calc_intuitions(shoal, num_cells_look);
                construct_inverted_table(shoal);
                current_group_ = find_group();
                (current_group_.size() + 1 >= group_threshold_) ? in_group_ = true : in_group_ = false;
            }

            bool in_group() const { return in_group_; }
            size_t group_size() const { return current_group_.size(); }

        private:
            std::vector<GroupZebrafish> find_group() const
            {
                std::vector<GroupZebrafish> group;
                std::vector<int> pos;
                // look for group forward
                boost::push_back(pos,
                    boost::irange(this->position_,
                        this->position_ + this->heading_ * static_cast<int>(group_cells_forward_) + this->heading_,
                        this->heading_));
                // look for group backward
                boost::push_back(pos,
                    boost::irange(this->position_ + (-this->heading_ * static_cast<int>(group_cells_backward_)),
                        this->position_, this->heading_));
                std::for_each(pos.begin(), pos.end(), [&](int& v) {
                    if (v < 0)
                        v += Params::fish_in_ring::num_cells;
                    else
                        v %= static_cast<int>(this->num_cells_);
                });

                for (int p : pos) {
                    if (ipos_.find(p) == ipos_.end())
                        continue;
                    group.insert(group.end(), ipos_.at(p).begin(), ipos_.at(p).end());
                }
                return group;
            }

            template <typename Shoal> void construct_inverted_table(const Shoal& s)
            {
                ipos_.clear();
                for (const GroupZebrafish& f : s) {
                    if (f.id() == this->id_)
                        continue;
                    ipos_[f.position()].push_back(f.shallow_clone());
                }
            }

            int group_heading() const
            {
                int sum = 0;
                for (const auto& f : current_group_)
                    sum += f.heading();
                return sum;
            }

            std::vector<std::pair<int, int>> group_sort_positions() const
            {
                std::vector<std::pair<int, int>> pairs;
                for (const auto& p : ipos_)
                    pairs.push_back(std::make_pair(p.first, p.second.size()));
                std::sort(
                    pairs.begin(), pairs.end(), [](const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
                        return lhs.second > rhs.second;
                    });
                return pairs;
            }

            bool in_group_;
            size_t group_threshold_;
            size_t group_cells_forward_;
            size_t group_cells_backward_;
            float social_influence_;

            std::vector<GroupZebrafish> current_group_;
            InvertedFishTable ipos_;
        };
    } // namespace fish
} // namespace samsar

#endif
