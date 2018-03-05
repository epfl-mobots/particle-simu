#ifndef ZEBRAFISH_HPP
#define ZEBRAFISH_HPP

#include <fish/fish_base.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace samsar {
    namespace fish {
        template <typename Params> class Zebrafish : public FishBase<Params> {
        public:
            Zebrafish() : next_heading_(Heading::UNDEFINED)
            {
                num_cells_ = Params::fish_in_ring::num_cells;
                max_neighbors_ = Params::fish_in_ring::max_neighbors;
                prob_move_ = Params::fish_in_ring::prob_move;
                prob_stay_ = Params::fish_in_ring::prob_stay;
                prob_obey_ = Params::fish_in_ring::prob_obey;
                heading_robot_ = Params::fish_in_ring::heading_robot;
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
                    this->position() = (this->position() + this->heading()) % static_cast<int>(num_cells_);
                    if (this->position_ < 0)
                        this->position_ += num_cells_;
                }
            }

            template <typename Shoal> void calc_intuitions(const Shoal& shoal, size_t num_cells_look)
            {
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
                        // TODO: we should insert more criteria for the fish selection
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
