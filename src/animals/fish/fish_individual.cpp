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
                int num_cells = fsim->sim_settings().get_field<int>("num_cells")->value();
                int cells_forward
                    = ff->individual_settings().get_field<int>("cells_forward")->value();
                int cells_backward
                    = ff->individual_settings().get_field<int>("cells_backward")->value();

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

        FishIndividual::FishIndividual()
        {
            _individual_settings
                // .add_setting("max_neighbors", 3)
                .add_setting("prob_obey", 1.0f)
                .add_setting("prob_move", 0.9f)
                .add_setting("min_speed", 1)
                .add_setting("max_speed", 1)
                .add_setting("group_threshold", 3)
                .add_setting("cells_forward", 5)
                .add_setting("cells_backward", 5)
                .add_setting("sum_weight", std::vector<float>{0.3f, -2.0f})
                .add_setting("heading_change_duration", 2);
            _init();
        }

        FishIndividual::FishIndividual(const Settings& settings)
        {
            _individual_settings = settings;
            _init();
        }

        FishIndividual::~FishIndividual() {}

        void FishIndividual::force_init() { _init(); }

        void FishIndividual::_init()
        {
            _individual_settings.add_setting(
                "initial_prob_obey", _individual_settings.get_field<float>("prob_obey")->value());
            _speed.max_speed = _individual_settings.get_field<int>("max_speed")->value();
            _speed.min_speed = _individual_settings.get_field<int>("min_speed")->value();
            _next_heading = Heading::UNDEFINED;
            _heading = random_heading();
            _speed.current = tools::random_in_range(_speed.min_speed, _speed.max_speed);
            _heading_change = false;
            _heading_change_time = 0;
        }

        void FishIndividual::stimulate(const std::shared_ptr<Simulation> sim)
        {
            _my_group(sim);
            _social_influence(sim);

            float prob_obey = _individual_settings.get_field<float>("prob_obey")->value();

            if (_heading_change) {
                if (_heading_change_time++
                    >= _individual_settings.get_field<int>("heading_change_duration")->value()) {
                    _heading_change_time = 0;
                    _heading_change = false;
                }
                else
                    return;
            }

            if (_my_group_idcs.size() > 0) {
                FishGroup fg(_my_group_idcs);
                _next_heading
                    = to_heading(fg.weighted_heading(sim, std::make_shared<FishIndividual>(*this),
                        std::make_shared<WeightFunc>(
                            _individual_settings.get_field<std::vector<float>>("sum_weight")
                                ->value())));

                if (tools::random_in_range(0.0f, 1.0f)
                    < 1 - _individual_settings.get_field<float>("prob_obey")->value()) {
                    _next_heading = reverse_heading(
                        fg.sum_heading(std::static_pointer_cast<FishSimulation>(sim)->fish()));
                    _heading_change = true;
                }
            }
            else {
                _next_heading = _heading;
                if (tools::random_in_range(0.0f, 1.0f) < 1 - prob_obey) {
                    _next_heading = reverse_heading(_heading);
                    _heading_change = true;
                }
            }

            if (_next_heading == Heading::UNDEFINED)
                _next_heading = to_heading(random_heading());
        }

        void FishIndividual::move(const std::shared_ptr<Simulation> sim)
        {
            _heading = _next_heading;

            if (_heading_change && _heading_change_time == 1)
                return; // heading change costs one timestep

            if (tools::random_in_range(0.0f, 1.0f)
                < 1 - _individual_settings.get_field<float>("prob_move")->value())
                return;

            int num_cells = sim->sim_settings().get_field<int>("num_cells")->value();
            _position.x = (_position.x + _speed.current * _heading) % num_cells;
            if (_position.x < 0)
                _position.x += num_cells;
        }

        void FishIndividual::_my_group(const std::shared_ptr<Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<FishSimulation>(sim);
            std::vector<FishIndividualPtr> fish = fsim->fish();

            int num_cells = fsim->sim_settings().get_field<int>("num_cells")->value();
            int group_threshold = _individual_settings.get_field<int>("group_threshold")->value();
            int cells_forward = _individual_settings.get_field<int>("cells_forward")->value();
            int cells_backward = _individual_settings.get_field<int>("cells_backward")->value();

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

        void FishIndividual::_social_influence(const std::shared_ptr<Simulation> sim)
        {
            auto fsim = std::static_pointer_cast<FishSimulation>(sim);
            std::vector<FishIndividualPtr> fish = fsim->fish();

            int num_cells = fsim->sim_settings().get_field<int>("num_cells")->value();
            int cells_forward = _individual_settings.get_field<int>("cells_forward")->value();

            if (_my_group_idcs.size() == 0) {
                _individual_settings.get_field<float>("prob_obey")->value()
                    = _individual_settings.get_field<float>("initial_prob_obey")->value();
                return;
            }

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

            _individual_settings.get_field<float>("prob_obey")->value()
                = _individual_settings.get_field<float>("initial_prob_obey")->value()
                * (1 - 1.0f / static_cast<float>(std::pow(neighs + 1, 4)));
        }

        Heading FishIndividual::heading() const { return _heading; }
        Heading& FishIndividual::heading() { return _heading; }
        Heading FishIndividual::next_heading() const { return _next_heading; }
        Heading& FishIndividual::next_heading() { return _next_heading; }
        size_t FishIndividual::group_size() const { return _my_group_idcs.size(); }

    } // namespace simulation
} // namespace samsar
