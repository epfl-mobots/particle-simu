#ifndef FISH_IN_RING_EXT_HPP
#define FISH_IN_RING_EXT_HPP

#include <simulation/fish_in_ring.hpp>
#include <types/fish/group.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

#include <limits>

namespace samsar {
    namespace defaults {
        struct ring_ext {
            static constexpr size_t group_min_size = 3;
            static constexpr size_t group_cells_forward = 1;
            static constexpr size_t group_cells_backward = 1;
        };
    } // namespace defaults

    namespace sim {

        template <typename Params, typename FishType, typename Stat = stat::StatBase<Params>>
        class FishInRingExt : public FishInRing<Params, FishType, Stat> {

            using base_type_t = SimulationBase<Params, Stat>;

        public:
            FishInRingExt()
                : group_min_size_(Params::ring_ext::group_min_size),
                  group_cells_forward_(Params::ring_ext::group_cells_forward),
                  group_cells_backward_(Params::ring_ext::group_cells_backward)
            {
                // we initialize the fish positions so that they are already within group range,
                // thus we don't have to deal with the setup time that is required for the fish
                // to group in the first place. In real experiments the fish are left to socialize
                // for some time before we start observing.
                int random_pos = tools::random_in_range(0.0f, 1.0f) * this->num_cells_;
                for (auto& f : this->fish_) {
                    int sgn = tools::random_sgn();
                    int pos;
                    (sgn < 0) ? pos = tools::random_in_range(0, static_cast<int>(group_cells_backward_))
                              : pos = tools::random_in_range(0, static_cast<int>(group_cells_forward_));
                    f.position() = tools::random_in_range(0, pos) + random_pos;
                }
            }

            void spin_once() override
            {
                using namespace types;

                this->update_stats(this);

                construct_groups();
                for (auto& f : this->fish_)
                    f.update_group_info(fish_groups_);

                for (auto& f : this->fish_)
                    f.calc_intuitions(this->fish_, this->num_cells_look_);

                for (auto& f : this->fish_)
                    f.move();

                base_type_t::spin_once();
            }

        private:
            void construct_groups()
            {
                using namespace types;
                fish_groups_.clear();

                auto ift = InvertedFishTable()(this->fish_);
                //                int last_candidate_pos = -1;
                for (const auto& v : ift) {
                    //                    if ((last_candidate_pos > 0)
                    //                        && (v.first - last_candidate_pos <=
                    //                        static_cast<int>(group_cells_forward_))) { last_candidate_pos = v.first;
                    //                        continue;
                    //                    }
                    //                    last_candidate_pos = v.first;

                    std::vector<int> pos;
                    boost::push_back(
                        pos, boost::irange(v.first, v.first + static_cast<int>(group_cells_forward_) + 1, 1));
                    boost::push_back(pos, boost::irange(v.first - static_cast<int>(group_cells_backward_), v.first, 1));

                    std::for_each(pos.begin(), pos.end(), [&](int& v) {
                        if (v < 0)
                            v += this->num_cells_;
                        else
                            v %= static_cast<int>(this->num_cells_);
                    });

                    std::vector<FishType> candidate_group;
                    for (int p : pos) {
                        if (ift.find(p) == ift.end())
                            continue;
                        candidate_group.insert(candidate_group.end(), ift.at(p).begin(), ift.at(p).end());
                    }

                    if (candidate_group.size() >= group_min_size_)
                        fish_groups_.push_back(Group<FishType>(candidate_group));
                }
                if (fish_groups_.size() > 0)
                    std::sort(fish_groups_.begin(), fish_groups_.end(),
                        [](const Group<FishType>& lhs, Group<FishType>& rhs) { return lhs.size() > rhs.size(); });
            }

            std::vector<types::Group<FishType>> fish_groups_;
            const size_t group_min_size_;
            const size_t group_cells_forward_;
            const size_t group_cells_backward_;
        };

    } // namespace sim
} // namespace samsar

#endif
