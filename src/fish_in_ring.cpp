#include <fish.hpp>
#include <fish_in_ring_sim.hpp>
#include <iostream>

struct Params {
    struct fish_in_ring {
        static constexpr size_t num_fish = 5;
        static constexpr size_t num_robot = 1;

        static constexpr int max_neighbors = 3;
        static constexpr size_t num_cells = 40;
        static constexpr int deg_vision = 120;
        static constexpr types::Heading heading_robot = types::Heading::CLOCKWISE;

        static constexpr float prob_obedience = 0.9f;
        static constexpr float prob_stay = 0.9f;
        static constexpr float prob_move = 1;
    };

    struct simulation {
        static constexpr uint64_t sim_time = 1800;
    };
};

int main()
{
    sim::FishInRing<Params, agent::Fish<Params>> fish_in_ring;
    //    fish_in_ring.spin_once();
    fish_in_ring.spin();
    return 0;
}
