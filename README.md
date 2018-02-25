#  __Fish in Ring experiment__

This is a C++11 implementation of the fish in ring experiment as described in "[Fish in a ring: spatio-temporal pattern formation in one-dimensional animal groups](http://rsif.royalsocietypublishing.org/content/7/51/1441#aHR0cDovL3JzaWYucm95YWxzb2NpZXR5cHVibGlzaGluZy5vcmcvY29udGVudC83LzUxLzE0NDEuZnVsbC5wZGZAQEAw)", based on Dr. Collignon's Matlab implementation.

You will also find python scripts that will allow you to plot the simulation's results.

\* *__This is a quick 'n dirty implementation that could be good enough to try a few optimization algorithms.__* \*

## Fish in ring simulation parameters

To alter the simulation's outcome and fish's behavior, you should change the parameters in ```src/examples/ring_example.cpp```.

```cpp
struct Params {
    struct fish_in_ring {
        static constexpr size_t num_fish = 5;
        static constexpr size_t num_robot = 1;

        static constexpr int max_neighbors = 3;
        static constexpr size_t num_cells = 40;
        static constexpr int deg_vision = 120;
        static constexpr types::Heading heading_robot = types::Heading::CLOCKWISE;

        static constexpr float prob_obedience = 0.9f;
        static constexpr float prob_stay = 0.901f;
        static constexpr float prob_move = 1;
    };

    struct simulation : defaults::simulation {
        static constexpr uint64_t sim_time = 1800;
    };
};
```

### Dependencies

* libboost-dev

#### Build project

This project uses the [Waf build system](http://www.waf.io).

```shell
./waf configure
./waf -j
 ```

#### Run a simulation

```shell
./build/fish_in_ring
 ```

 After each run, a timestamped folder containing the simulation results will be created in the root project directory.

 ## Plotting the results

 ### Dependencies

You should install the following python modules:

* ```matplotlib``` -- ```sudo pip install matplotlib```
* ```numpy``` -- ```sudo pip install matplotlib```
* ```termcolor``` -- ```sudo pip install matplotlib```

### Scripts

* plot_fish_in_ring.py

    A visual representation of the ring and fish's movements. Type ```plot_fish_in_ring.py -h``` for more information about the script.

* plot_polarities.py

    Plots the polarity per timestep for a simulation run. Type ```plot_polarities.py -h``` for more information about the script.

* plot_proportion_heading.py

    Plots the proportion of time the shoal spent in a specified heading. Type ```plot_proportion_heading.py -h``` for more information about the script.

#### Usage

```shell
./<script> <simulation results path>
```
