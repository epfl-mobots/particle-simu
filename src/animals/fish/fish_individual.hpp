#ifndef FISH_INDIVIDUAL_HPP
#define FISH_INDIVIDUAL_HPP

#include "heading.hpp"
#include <simulation/individual.hpp>

namespace samsar {
    namespace simulation {
        using namespace types;

        struct FishParams {
            float prob_obey = 0.94f;
            float prob_move = 0.901f;
            float prob_change_speed = 0.0f;
            int group_threshold = 3;
            int cells_forward = 5;
            int cells_backward = 8;
            int min_speed = 1;
            int max_speed = 1;
            std::vector<float> sum_weight = {0.3f, -0.6f};
            int heading_change_duration = 3;
            int influence_alpha = 4;
            Heading heading_bias = Heading::UNDEFINED;

            void print() const
            {
                std::cout << "prob_obey: " << prob_obey << std::endl;
                std::cout << "prob_move: " << prob_move << std::endl;
                std::cout << "prob_change_speed: " << prob_change_speed << std::endl;
                std::cout << "group_threshold: " << group_threshold << std::endl;
                std::cout << "cells_forward: " << cells_forward << std::endl;
                std::cout << "cells_backward: " << cells_backward << std::endl;
                std::cout << "min_speed: " << min_speed << std::endl;
                std::cout << "max_speed: " << max_speed << std::endl;
                std::cout << "sum_weight: " << sum_weight[0] << " " << sum_weight[1] << std::endl;
                std::cout << "heading_change_duration: " << heading_change_duration << std::endl;
                std::cout << "influence_alpha: " << influence_alpha << std::endl;
                std::cout << "heading_bias: " << heading_bias << std::endl;
            }
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

            virtual void force_init();

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
