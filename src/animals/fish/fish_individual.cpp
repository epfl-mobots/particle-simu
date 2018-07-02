#include "fish_individual.hpp"
#include "group.hpp"

#include "fish_simulation.hpp"
#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>

namespace samsar {

    namespace types {
        struct WeightFunc : public defaults::WeightFunc {
            WeightFunc(const std::vector<float> w) : defaults::WeightFunc(w)
            {
                assert(w.size() == 2);
            }

            float operator()(const std::shared_ptr<Simulation> sim, const FishIndividualPtr& ff,
                const FishIndividualPtr& f) const override
            {
                auto fsim = std::static_pointer_cast<FishSimulation>(sim);
                int num_cells = fsim->fish_sim_settings().num_cells;
                int cells_forward = ff->fish_params().cells_forward;
                int cells_backward = ff->fish_params().cells_backward;

                std::vector<int> forward;
                boost::push_back(forward,
                    boost::irange(ff->position().x + ff->heading() * cells_forward,
                        ff->position().x - ff->heading(), -ff->heading()));
                std::vector<int> backward;
                boost::push_back(backward,
                    boost::irange(ff->position().x - ff->heading(),
                        ff->position().x + (-ff->heading() * cells_backward) - ff->heading(),
                        -ff->heading()));
                std::for_each(forward.begin(), forward.end(),
                    [&](int& v) { (v < 0) ? v += num_cells : v %= num_cells; });
                std::for_each(backward.begin(), backward.end(),
                    [&](int& v) { (v < 0) ? v += num_cells : v %= num_cells; });

                const auto itf = std::find(forward.begin(), forward.end(), f->position().x);
                if (!(forward.end() == itf)) {
                    auto idx = std::distance(forward.begin(), itf);
                    return std::exp(_w[0] * idx);
                }

                const auto itb = std::find(backward.begin(), backward.end(), f->position().x);
                if (!(backward.end() == itb)) {
                    auto idx = std::distance(backward.begin(), itb);
                    return std::exp(_w[1] * idx);
                }
                return 0;
            }
        };
    } // namespace types

    namespace simulation {

        FishIndividual::FishIndividual() { _init(); }

        FishIndividual::FishIndividual(const FishParams& params)
        {
            _fish_params = params;
            _init();
        }

        FishIndividual::~FishIndividual() {}

        void FishIndividual::force_init() { _init(); }

        void FishIndividual::_init()
        {
            _next_heading = Heading::UNDEFINED;
            _heading = random_heading();
            _heading_change = false;
            _heading_change_time = 0;
            _speed.current = tools::random_in_range(_fish_params.min_speed, _fish_params.max_speed);
            _speed.max_speed = _fish_params.max_speed;
            _speed.min_speed = _fish_params.min_speed;
        }

        void FishIndividual::stimulate(const std::shared_ptr<Simulation> sim)
        {
            _my_group(sim);
            float prob_obey = _social_influence(sim);

            if (_heading_change) {
                if (_heading_change_time++ >= _fish_params.heading_change_duration) {
                    _heading_change_time = 0;
                    _heading_change = false;
                }
                else
                    return;
            }

            // heading
            if (_my_group_idcs.size() > 0) {
                FishGroup fg(_my_group_idcs);
                _next_heading
                    = to_heading(fg.weighted_heading(sim, std::make_shared<FishIndividual>(*this),
                        std::make_shared<WeightFunc>(_fish_params.sum_weight)));

                if (tools::random_in_range(0.0f, 1.0f) < 1 - prob_obey) {
                    if (_fish_params.heading_bias == Heading::UNDEFINED) {
                        _next_heading = reverse_heading(
                            fg.sum_heading(std::static_pointer_cast<FishSimulation>(sim)->fish()));
                    }
                    else
                        _next_heading = _fish_params.heading_bias;
                    _heading_change = true;
                }

                // mimic group speed
                _speed.set_speed_bounded(static_cast<int>(
                    std::round(fg.average_speed(sim, std::make_shared<FishIndividual>(*this)))));
            }
            else {
                if (_fish_params.heading_bias == Heading::UNDEFINED) {
                    _next_heading = _heading;
                    if (tools::random_in_range(0.0f, 1.0f) < 1 - prob_obey) {
                        _next_heading = reverse_heading(_heading);
                        _heading_change = true;
                    }
                }
                else {
                    _next_heading = _fish_params.heading_bias;
                }
            }

            // speed
            if (tools::random_in_range(0.0f, 1.0f) < _fish_params.prob_change_speed) {
                _speed.current
                    = tools::random_in_range(_fish_params.min_speed, _fish_params.max_speed);
            }

            if (_next_heading == Heading::UNDEFINED) {
                if (_fish_params.heading_bias == Heading::UNDEFINED)
                    _next_heading = to_heading(random_heading());
                else
                    _next_heading = _fish_params.heading_bias;
            }
        }

        void FishIndividual::move(const std::shared_ptr<Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<FishSimulation>(sim);

            _heading = _next_heading;

            if (_heading_change && _heading_change_time == 1)
                return; // heading change costs one timestep

            if (tools::random_in_range(0.0f, 1.0f) < 1 - _fish_params.prob_move)
                return;

            int num_cells = fsim->fish_sim_settings().num_cells;
            _position.x = (_position.x + _speed.current * _heading) % num_cells;
            if (_position.x < 0)
                _position.x += num_cells;
        }

        void FishIndividual::_my_group(const std::shared_ptr<Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<FishSimulation>(sim);
            std::vector<FishIndividualPtr> fish = fsim->fish();

            int num_cells = fsim->fish_sim_settings().num_cells;
            int group_threshold = _fish_params.group_threshold;
            int cells_forward = _fish_params.cells_forward;
            int cells_backward = _fish_params.cells_backward;

            std::vector<int> pos;
            boost::push_back(pos,
                boost::irange(
                    _position.x + 1, _position.x + _heading * cells_forward + _heading, _heading));
            boost::push_back(pos,
                boost::irange(
                    _position.x + (-_heading * cells_backward), _position.x + _heading, _heading));
            std::for_each(
                pos.begin(), pos.end(), [&](int& v) { (v < 0) ? v += num_cells : v %= num_cells; });

            std::vector<size_t> candidate;
            auto ipos = InvertedFishTable()(fish);
            for (const auto& p : ipos) {
                auto result = std::find(pos.begin(), pos.end(), p.first);
                if (result == pos.end())
                    continue;
                candidate.insert(candidate.end(), p.second.begin(), p.second.end());
            }

            _my_group_idcs.clear();
            if (candidate.size() >= static_cast<size_t>(group_threshold))
                _my_group_idcs = candidate;
        }

        float FishIndividual::_social_influence(const std::shared_ptr<Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<FishSimulation>(sim);
            std::vector<FishIndividualPtr> fish = fsim->fish();

            int num_cells = fsim->fish_sim_settings().num_cells;
            int cells_forward = _fish_params.cells_forward;

            if (_my_group_idcs.size() == 0)
                return _fish_params.prob_obey;

            // we take into account the fish that are located in front of the focal fish,
            // i.e. in its field of view. Fish that do not see a lot of neighbors in front
            // of them have higher probability to change direction, while ones that have
            // a lot of fish in front of them, are less prone to disobey the group.
            std::vector<int> pos;
            boost::push_back(pos,
                boost::irange(
                    _position.x, _position.x + _heading * cells_forward + _heading, _heading));
            std::for_each(
                pos.begin(), pos.end(), [&](int& v) { (v < 0) ? v += num_cells : v %= num_cells; });

            int neighs = 0;
            auto ipos = InvertedFishTable()(fish);
            for (int p : pos) {
                if (ipos.find(p) == ipos.end())
                    continue;
                ++neighs; // we count the focal fish
            }

            int influence_alpha = _fish_params.influence_alpha;
            return _fish_params.prob_obey
                * (1 - 1.0f / static_cast<float>(std::pow(neighs + 1, influence_alpha)));
        }

        Heading FishIndividual::heading() const { return _heading; }
        Heading& FishIndividual::heading() { return _heading; }
        Heading FishIndividual::next_heading() const { return _next_heading; }
        Heading& FishIndividual::next_heading() { return _next_heading; }
        size_t FishIndividual::group_size() const { return _my_group_idcs.size(); }
        FishParams& FishIndividual::fish_params() { return _fish_params; }
        FishParams FishIndividual::fish_params() const { return _fish_params; }
    } // namespace simulation
} // namespace samsar
