#  __Simple Collective Behavior Simulator__

This is a C++11 implementation of a simplistic collective behavior simulator, that will allow us to run simulations that aim to __approximate__ the collective behavior of a set of animals.

You will also find python scripts that will allow you to plot the simulation's results.

\* *__This is an early implementation that could be good enough to try a few optimization algorithms.__* \*

## Fish in ring simulation parameters

To alter the simulation's outcome and fish's behavior, you should change the parameters of the ```FishIndividual``` object, in one of the following ways *__(for now check the default parameters in the default constructor definition)__*:

```cpp
FishIndividual f1;
f1.fish_settings().get_field<float>("prob_obey")->value() = 0.94f;
f1.fish_settings().get_field<float>("prob_move")->value() = 0.901f;
f1.fish_settings().get_field<int>("max_speed")->value() = 2;

Settings settings_for_fish;
settings_for_fish.add_setting("max_neighbors", 3)
    .add_setting("prob_obey", 1.0f)
    .add_setting("prob_move", 0.9f)
    .add_setting("min_speed", 1)
    .add_setting("max_speed", 1)
    .add_setting("group_threshold", 3)
    .add_setting("cells_forward", 5)
    .add_setting("cells_backward", 5)
    .add_setting("sum_weight", std::vector<float>{0.3f, -2.0f})
    .add_setting("heading_robot", Heading::CLOCKWISE)
    .add_setting("heading_change_duration", 2);
FishIndividual f2(settings_for_fish);
```

To customize the statistics output, you can add different statistics callbacks that will be invoked at the end of each timestep.

```cpp
FishSimulation sim;
sim.add_stat(std::make_shared<PositionStat>())
    .add_stat(std::make_shared<PolarityStat>())
    .add_stat(std::make_shared<PositionStat>())
    .add_stat(std::make_shared<HeadingStat>())
    .add_stat(std::make_shared<GroupStat>())
    .add_stat(std::make_shared<GroupParamsStat>())
    .add_stat(std::make_shared<FishInRingParamsStat>());
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
./build/fish_ring_example
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

* more scripts are/might be included, please use the -h parameter to view information concerning the usage.

#### Usage

```shell
./<script> <simulation results path>
```
