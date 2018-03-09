#ifndef SIMPLISTIC_ZEBRAFISH_HPP
#define SIMPLISTIC_ZEBRAFISH_HPP

#include <fish/fish_base.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

#include <random/random_generator.hpp>

namespace samsar {
    namespace defaults {
        struct zebrafish {
            static constexpr types::Heading heading_robot = types::Heading::CLOCKWISE;

            static constexpr int max_neighbors = 3;

            static constexpr float prob_obey = 0.9f;
            static constexpr float prob_stay = 0.901f;
            static constexpr float prob_move = 1.0f;

            static constexpr int min_speed = 1;
            static constexpr int max_speed = 3;
        };
    } // namespace defaults

    namespace fish {

        template <typename Params> class SimplisticZebrafish : public FishBase<Params> {
        public:
            SimplisticZebrafish(size_t num_cells)
                : num_cells_(num_cells),
                  max_neighbors_(Params::zebrafish::max_neighbors),
                  prob_move_(Params::zebrafish::prob_move),
                  prob_stay_(Params::zebrafish::prob_stay),
                  prob_obey_(Params::zebrafish::prob_obey),
                  heading_robot_(Params::zebrafish::heading_robot),
                  next_heading_(Heading::UNDEFINED)

            {
            }

            void move() override
            {
                if (this->heading_ == Heading::UNDEFINED)
                    return;

                // Update the fish's heading according to the neighbors'
                // collective heading
                this->heading_ = next_heading_;

                // a fish will  conform to the neighbors' heading with some
                // uncertainty, i.e. the fish might in fact disobey and
                // move on the opposite heading with prob_obedience probability
                if (!this->is_robot()) {
                    bool obey = tools::random_in_range(0.0f, 1.0f) < prob_obey_;
                    if (!obey)
                        this->heading() = reverse_heading(this->heading());
                }
                else
                    // for now robotic fish will explicitly stick to their heading
                    this->heading_ = heading_robot_;

                // a fish will move to the next position with some uncertainty,
                // i.e. this means the fish might not move at all
                bool move;
                (this->is_robot()) ? (move = tools::random_in_range(0.0f, 1.0f) < prob_move_)
                                   : (move = tools::random_in_range(0.0f, 1.0f) < (1 - prob_stay_));
                if (move) {
                    this->position()
                        = (this->position() + this->heading() * this->speed()) % static_cast<int>(num_cells_);
                    if (this->position_ < 0)
                        this->position_ += num_cells_;
                }
            }

            template <typename Shoal> void calc_intuitions(const Shoal& shoal, size_t num_cells_look)
            {
                this->speed_ = tools::random_in_range<int>(this->min_speed_, this->max_speed_);

                // calculate the neighborhoods according to the agent's heading
                // and the problem parameters:
                // degrees of vision + cell degrees = number of cells to look
                std::vector<int> neighborhood;
                boost::push_back(neighborhood,
                    boost::irange(this->position_,
                        this->position_ + this->heading_ * static_cast<int>(num_cells_look) + this->heading_,
                        this->heading_));
                std::for_each(neighborhood.begin(), neighborhood.end(), [&](int& v) {
                    if (v < 0)
                        v += num_cells_;
                    else
                        v %= static_cast<int>(num_cells_);
                });

                // iterate through all the cells the fish can look for neighbors
                // until the max neighbor criteria is met or the fish runs out of cells
                size_t num_neighbors = 0;
                int sum_heading = 0;
                for (size_t j = 0; j <= num_cells_look; ++j) {
                    // find the fish that belong to the jth neighborhood
                    std::vector<size_t> neighbors = find_neighbors(shoal, neighborhood.at(j), this->id_);
                    if (neighbors.size() > 0) {
                        // pick the neighbors in a random way
                        std::shuffle(neighbors.begin(), neighbors.end(), tools::RandomGenerator().gen());
                        for (size_t k = 0; k < neighbors.size(); ++k) {
                            ++num_neighbors;
                            sum_heading += shoal[neighbors.at(k)].heading();
                            if (num_neighbors == max_neighbors_)
                                break;
                        }
                    }
                    if (num_neighbors == max_neighbors_)
                        break;
                } // for j (num_cells_look_)

                // Intuition about the next heading - No probabilistic decisions at this point
                // -- calculate the new heading using both the fish's heading
                // -- and the collective heading (sum of headings)
                Heading new_heading = to_heading(this->heading_ + sum_heading);
                if (new_heading == Heading::UNDEFINED)
                    // if the collective heading is balanced between clockwise and
                    // counter-clockwise, then we assume the fish will keep its heading
                    new_heading = this->heading_;
                next_heading_ = new_heading;
            }

            size_t& num_cells() { return num_cells_; }
            size_t& max_neighbors() { return max_neighbors_; }
            float& prob_move() { return prob_move_; }
            float& prob_stay() { return prob_stay_; }
            float& prob_obey() { return prob_obey_; }
            Heading& heading_robot() { return heading_robot_; }

        protected:
            template <typename Shoal>
            std::vector<size_t> find_neighbors(const Shoal& shoal, int neighborhood, size_t self) const
            {
                std::vector<size_t> neighbors;
                for (size_t i = 0; i < shoal.size(); ++i) {
                    if (i == self)
                        continue;
                    if (shoal[i].position() == neighborhood)
                        neighbors.push_back(i);
                }
                return neighbors;
            }

            size_t num_cells_;
            size_t max_neighbors_;
            float prob_move_;
            float prob_stay_;
            float prob_obey_;
            Heading heading_robot_;
            Heading next_heading_;
        };

    } // namespace fish
} // namespace samsar

#endif
