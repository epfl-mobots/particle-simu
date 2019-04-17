#  particle-simu 

This is a C++11 implementation of a simplistic collective behaviour simulator. The goal of this project is to provide a testbed for __approximating__  collective interaction dynamics before deploying the models to our robotic system.

# Usage

In the `src/` you will find the higher level functionality of the simulator (e.g., a spinner, archive functions, library specific types, random generators, etc)

One can make use of the code in the src to create custom simulation loops. To do so:

- Create an experiment directory
    
    `./waf --create zebrafish_model`
    
   You will see a new folder `animals` under which you can find your new experiment's folder `zebrafish_model`. The directory will contain a `wscript` template to help you with the compilation process.
   
- Configure the environment for your project:
  
   `./waf configure --animal zebrafish_model`

- Compile your project:
  
   `./waf --animal zebrafish_model`
   

# Examples
You can find an example project for burst-and-coast simulation [here](https://github.com/epfl-mobots/burst-and-coast)
   
   


## *__This is a work in progress, hence the limited documentation.__* 
