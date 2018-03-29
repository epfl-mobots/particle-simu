#ifndef FISH_INDIVIDUAL_HPP
#define FISH_INDIVIDUAL_HPP

#include "heading.hpp"
#include <settings/settings.hpp>
#include <simulation/individual.hpp>

namespace samsar {
    namespace simulation {
        using namespace types;

        class FishIndividual : public Individual<int, int> {
        public:
            FishIndividual();
            FishIndividual(const Settings& settings);
            virtual ~FishIndividual();

            virtual void stimulate(const std::shared_ptr<Simulation>) override;
            virtual void move(const std::shared_ptr<Simulation>) override;

            Heading heading() const;
            Heading& heading();
            Heading next_heading() const;
            Heading& next_heading();
            size_t group_size() const;

            void force_init();

        protected:
            void _init();
            void _my_group(const std::shared_ptr<Simulation> sim);
            void _social_influence(const std::shared_ptr<Simulation> sim);

            std::vector<size_t> _my_group_idcs;
            Heading _heading;
            Heading _next_heading;

            bool _heading_change;
            int _heading_change_time;
        };

        using FishIndividualPtr = std::shared_ptr<FishIndividual>;

    } // namespace simulation
} // namespace samsar

#endif
