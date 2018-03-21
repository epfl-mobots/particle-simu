#ifndef SOCIAL_ZEBRAFISH_HPP
#define SOCIAL_ZEBRAFISH_HPP

#include <fish/fish_base.hpp>
#include <types/fish/group.hpp>
#include <types/movement/desired_position.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace samsar {
    namespace defaults {
        struct social_zebrafish {
            static constexpr int min_speed = 1;
            static constexpr int max_speed = 1;

            static constexpr size_t group_threshold = 3;
            static constexpr size_t cells_forward = 5;
            static constexpr size_t cells_backward = 5;

            static constexpr float prob_obey = 1.0f;
        };
    } // namespace defaults

    namespace fish {

        template <typename Params> class SocialZebrafish : public FishBase<Params> {

            using base_type_t = FishBase<Params>;
            using Group = types::Group<SocialZebrafish<Params>>;

        public:
            SocialZebrafish(size_t num_cells)
                : num_cells_(num_cells),
                  group_threshold_(Params::social_zebrafish::group_threshold),
                  cells_forward_(Params::social_zebrafish::cells_forward),
                  cells_backward_(Params::social_zebrafish::cells_backward),
                  prob_obey_(Params::social_zebrafish::prob_obey),
                  next_heading_(this->heading_)
            {
                this->min_speed() = Params::social_zebrafish::min_speed;
                this->max_speed() = Params::social_zebrafish::max_speed;
                this->speed() = this->min_speed();
            }

            SocialZebrafish(const SocialZebrafish<Params>& sz)
            {
                // this copy constructor explicitly skips the copying
                // of the current_group_ vector, otherwise an infinite
                // recursive copying will be invoked
                current_group_ = Group();

                this->heading_ = sz.heading();
                this->position_ = sz.position();
                this->is_robot_ = sz.is_robot();
                this->id_ = sz.id();
                this->min_speed_ = sz.min_speed();
                this->max_speed_ = sz.max_speed();
                this->speed_ = sz.speed();

                num_cells_ = sz.num_cells();
                group_threshold_ = sz.group_threshold();
                cells_forward_ = sz.cells_forward();
                cells_backward_ = sz.cells_backward();
                prob_obey_ = sz.prob_obey();
                next_heading_ = sz.next_heading();
            }

            void move() override
            {
                this->heading_ = next_heading_;
                this->position() = (this->position() + this->speed() * this->heading()) % static_cast<int>(num_cells_);
                if (this->position_ < 0)
                    this->position_ += num_cells_;
            }

            template <typename Fish> void calc_intuitions(const Fish& fish, size_t /*num_cells_look*/)
            {
                construct_group(fish);
                calc_prob_lead(fish);

                if (current_group_.size() > 0) {
                    next_heading_ = to_heading(current_group_.weighted_heading(*this));
                    if (tools::random_in_range(0.0f, 1.0f) < 1 - prob_obey_) {
                        next_heading_ = reverse_heading(current_group_.heading());
                    }
                }
                else {
                    next_heading_ = this->heading_;
                    if (tools::random_in_range(0.0f, 1.0f) < 1 - prob_obey_)
                        next_heading_ = reverse_heading(current_group_.heading());
                }

                if (next_heading_ == Heading::UNDEFINED)
                    next_heading_ = to_heading(random_heading());
            }

            template <typename FishVec> void construct_group(const FishVec& fish)
            {
                using namespace types;

                std::vector<int> pos;
                // look for group forward
                boost::push_back(pos,
                    boost::irange(this->position_,
                        this->position_ + this->heading_ * static_cast<int>(cells_forward_) + this->heading_,
                        this->heading_));
                // look for group backward
                boost::push_back(pos,
                    boost::irange(this->position_ + (-this->heading_ * static_cast<int>(cells_backward_)),
                        this->position_, this->heading_));
                std::for_each(pos.begin(), pos.end(),
                    [&](int& v) { (v < 0) ? v += this->num_cells_ : v %= static_cast<int>(this->num_cells_); });

                FishVec candidate;
                auto ipos = InvertedFishTable()(fish);
                for (int p : pos) {
                    if (ipos.find(p) == ipos.end())
                        continue;
                    candidate.insert(candidate.end(), ipos.at(p).begin(), ipos.at(p).end());
                }

                current_group_.clear();
                if (candidate.size() >= group_threshold_)
                    current_group_.set(candidate);
            }

            template <typename FishVec> void calc_prob_lead(const FishVec& fish)
            {
                if (current_group_.size() == 0) {
                    prob_obey_ = Params::social_zebrafish::prob_obey;
                    return;
                }

                // we take into account the fish that are located in front of the focal fish,
                // i.e. in its field of view. Fish that do not see a lot of neighbors in front
                // of them have higher probability to change direction, while ones that have
                // a lot of fish in front of them, are less prone to disobey the group.
                std::vector<int> pos;
                boost::push_back(pos,
                    boost::irange(this->position_,
                        this->position_ + this->heading_ * static_cast<int>(cells_forward_) + this->heading_,
                        this->heading_));
                std::for_each(pos.begin(), pos.end(),
                    [&](int& v) { (v < 0) ? v += this->num_cells_ : v %= static_cast<int>(this->num_cells_); });

                int neighs = 0;
                auto ipos = InvertedFishTable()(fish);
                for (int p : pos) {
                    if (ipos.find(p) == ipos.end())
                        continue;
                    ++neighs;
                }

                prob_obey_ = Params::social_zebrafish::prob_obey * (1 - 1.0f / std::pow(neighs + 2, 2));
            }

            size_t num_cells() const { return num_cells_; }
            size_t group_threshold() const { return group_threshold_; }
            size_t cells_forward() const { return cells_forward_; }
            size_t cells_backward() const { return cells_backward_; }
            float prob_obey() const { return prob_obey_; }
            Heading next_heading() const { return next_heading_; }

            size_t& num_cells() { return num_cells_; }
            size_t& group_threshold() { return group_threshold_; }
            size_t& cells_forward() { return cells_forward_; }
            size_t& cells_backward() { return cells_backward_; }
            float& prob_obey() { return prob_obey_; }
            Heading& next_heading() { return next_heading_; }

            Group current_group() const { return current_group_; }

        private:
            size_t num_cells_;
            size_t group_threshold_;
            size_t cells_forward_;
            size_t cells_backward_;
            float prob_obey_;
            Heading next_heading_;

            Group current_group_;
        }; // namespace fish
    } // namespace fish
} // namespace samsar

#endif
