#ifndef RUMMY_INDIVIDUAL_HPP
#define RUMMY_INDIVIDUAL_HPP

#include <simulation/individual.hpp>

namespace simu {
    namespace simulation {
        struct FishParams {

            void print() const
            {
            }
        };

        class RummyIndividual : public Individual<int, int> {
        public:
            RummyIndividual();
            RummyIndividual(const FishParams& params);
            virtual ~RummyIndividual();

            virtual void stimulate(const std::shared_ptr<Simulation>) override;
            virtual void move(const std::shared_ptr<Simulation>) override;

            FishParams& fish_params();
            FishParams fish_params() const;

        protected:
            FishParams _fish_params;
        };

        using RummyIndividualPtr = std::shared_ptr<RummyIndividual>;

    } // namespace simulation
} // namespace simu

#endif
