#ifndef FISHREPLAY_HPP
#define FISHREPLAY_HPP

#include <animals/fish/fish_individual.hpp>
#include <simulation/simulation.hpp>

namespace samsar {
    namespace actual {

        class FishReplay : public simulation::FishIndividual {
        public:
            FishReplay(const std::vector<int>& trajectory);
            virtual ~FishReplay() override;

            virtual void stimulate(const std::shared_ptr<simulation::Simulation>) override;
            virtual void move(const std::shared_ptr<simulation::Simulation>) override;

        private:
            std::vector<int> _trajectory;
        };

    } // namespace actual
} // namespace samsar

#endif
