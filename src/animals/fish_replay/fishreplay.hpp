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
            virtual void force_init() override;

            const std::vector<bool>& predicted_hdg() const { return _predicted_hdg; }
            const std::vector<bool>& predicted_move() const { return _predicted_move; }

        private:
            std::vector<int> _trajectory;
            std::vector<bool> _predicted_hdg;
            std::vector<bool> _predicted_move;

            void _init();
        };

        using FishReplayPtr = std::shared_ptr<FishReplay>;

    } // namespace actual
} // namespace samsar

#endif
