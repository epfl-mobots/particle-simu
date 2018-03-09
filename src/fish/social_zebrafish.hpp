#ifndef SOCIAL_ZEBRAFISH_HPP
#define SOCIAL_ZEBRAFISH_HPP

#include <fish/fish_base.hpp>
#include <types/fish/group.hpp>
#include <types/movement/desired_position.hpp>

namespace samsar {
    namespace defaults {
        struct social_zebrafish {
            static constexpr int min_speed = 1;
            static constexpr int max_speed = 2;

            static constexpr size_t attraction_disctance = 5;

            static constexpr float prob_obey = 0.99f;
            static constexpr float obedience_noise = 0.0f;
            static constexpr float leadership_noise = 0.01f;
        };
    } // namespace defaults

    namespace fish {

        template <typename Params> class SocialZebrafish : public FishBase<Params> {

            using base_type_t = FishBase<Params>;
            using Group = types::Group<SocialZebrafish<Params>>;

        public:
            SocialZebrafish(size_t num_cells)
                : attraction_distance_(Params::social_zebrafish::attraction_disctance),
                  num_cells_(num_cells),
                  prob_lead_(tools::random_in_range(0.0f, 1.0f)),
                  prob_obey_(Params::social_zebrafish::prob_obey),
                  obedience_noise_(Params::social_zebrafish::obedience_noise),
                  leadership_noise_(Params::social_zebrafish::leadership_noise),
                  in_group_(false)
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
                this->heading_ = sz.heading();
                this->position_ = sz.position();
                this->is_robot_ = sz.is_robot();
                this->id_ = sz.id();
                this->min_speed_ = sz.min_speed();
                this->max_speed_ = sz.max_speed();
                this->speed_ = sz.speed();
                attraction_distance_ = sz.attraction_distance();
                num_cells_ = sz.num_cells();
                next_heading_ = sz.next_heading();
                desired_pos_ = sz.desired_pos();
                in_group_ = sz.in_group();
                prob_lead_ = sz.prob_lead();
                prob_obey_ = sz.prob_obey();
                obedience_noise_ = sz.obedience_noise();
                leadership_noise_ = sz.leadership_noise();
            }

            void move() override
            {
                this->heading_ = next_heading_;
                this->position() = (this->position() + this->speed() * this->heading()) % static_cast<int>(num_cells_);
                if (this->position_ < 0)
                    this->position_ += num_cells_;
            }

            template <typename Shoal> void calc_intuitions(const Shoal& /*shoal*/, size_t /*num_cells_look*/)
            {
                prob_lead_ += tools::random_sgn() * tools::random_in_range(0.0f, leadership_noise_);
                prob_obey_ += tools::random_sgn() * tools::random_in_range(0.0f, obedience_noise_);
                if (in_group_) {
                    if (tools::random_in_range(0.0f, 1.0f) < (1 - prob_obey_)) {
                        // if a fish initiates a movement it is more likely
                        // to become a leader, therefore we re evalutate its
                        // probability as a leader
                        next_heading_ = reverse_heading(next_heading_);
                        prob_lead_ = tools::random_in_range(0.0f, 1.0f);
                    }
                    else
                        next_heading_ = current_group_.imposed_heading();
                }
                else if (!desired_pos_.empty())
                    next_heading_ = desired_pos_.heading_;
                else
                    next_heading_ = this->heading_;
            }

            template <typename FishGroups> void update_group_info(const FishGroups& fgroups)
            {
                using namespace types;
                in_group_ = false;
                current_group_ = Group();
                if (fgroups.size() == 0)
                    return;

                const auto& g = fgroups.at(0);
                if (g.has(this->id()) && (g.size() > 1)) {
                    desired_pos_.clear();
                    in_group_ = true;
                    current_group_ = g;
                    return;
                }
                else {
                    int distance = -1;
                    if (this->position_ + g.center_of_gravity() + 1 > static_cast<int>(num_cells_))
                        distance = std::abs(this->position_ - static_cast<int>(num_cells_))
                            + std::abs(g.center_of_gravity() - static_cast<int>(num_cells_));
                    else
                        distance = std::abs(this->position_ - g.center_of_gravity());
                    if (distance >= static_cast<int>(attraction_distance_))
                        desired_pos_ = DesiredPositionInfo(g.center_of_gravity(), reverse_heading(g.imposed_heading()));
                    else
                        desired_pos_ = DesiredPositionInfo(g.center_of_gravity(), random_heading());
                }
            }

            size_t attraction_distance() const { return attraction_distance_; }
            size_t num_cells() const { return num_cells_; }
            Heading next_heading() const { return next_heading_; }
            DesiredPositionInfo desired_pos() const { return desired_pos_; }
            bool in_group() const { return in_group_; }
            float prob_lead() const { return prob_lead_; }
            float prob_obey() const { return prob_obey_; }
            types::Group<SocialZebrafish<Params>> current_group() const { return current_group_; }
            float obedience_noise() const { return obedience_noise_; }
            float leadership_noise() const { return leadership_noise_; }

            size_t& attraction_distance() { return attraction_distance_; }
            size_t& num_cells() { return num_cells_; }
            Heading& next_heading() { return next_heading_; }
            Heading& desired_pos() { return desired_pos_; }
            bool& in_group() { return in_group_; }

        private:
            size_t attraction_distance_;
            size_t num_cells_;
            Heading next_heading_;
            float prob_lead_;
            float prob_obey_;
            float obedience_noise_;
            float leadership_noise_;

            Group current_group_;
            DesiredPositionInfo desired_pos_;
            bool in_group_;
        }; // namespace fish
    } // namespace fish
} // namespace samsar

#endif
