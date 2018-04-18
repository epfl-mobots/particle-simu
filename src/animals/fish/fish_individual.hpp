#ifndef FISH_INDIVIDUAL_HPP
#define FISH_INDIVIDUAL_HPP

#include "heading.hpp"
#include <simulation/individual.hpp>

namespace samsar {
    namespace simulation {
        using namespace types;

        struct FishParams {
            float prob_obey = 1.0f;
            float prob_move = 0.901f;
            float prob_change_speed = 0.1f;
            int group_threshold = 3;
            int cells_forward = 5;
            int cells_backward = 5;
            int min_speed = 1;
            int max_speed = 1;
            std::vector<float> sum_weight = {0.3f, -2.0f};
            int heading_change_duration = 2;
            int influence_alpha = 4;
        };

        class FishIndividual : public Individual<int, int> {
        public:
            FishIndividual();
            FishIndividual(const FishParams& params);
            virtual ~FishIndividual();

            virtual void stimulate(const std::shared_ptr<Simulation>) override;
            virtual void move(const std::shared_ptr<Simulation>) override;

            Heading heading() const;
            Heading& heading();
            Heading next_heading() const;
            Heading& next_heading();
            size_t group_size() const;

            void force_init();

            FishParams& fish_params();
            FishParams fish_params() const;

        protected:
            void _init();
            void _my_group(const std::shared_ptr<Simulation> sim);
            float _social_influence(const std::shared_ptr<Simulation> sim);

            std::vector<size_t> _my_group_idcs;
            Heading _heading;
            Heading _next_heading;

            bool _heading_change;
            int _heading_change_time;

            FishParams _fish_params;
        };

        using FishIndividualPtr = std::shared_ptr<FishIndividual>;

    } // namespace simulation
} // namespace samsar

#endif
