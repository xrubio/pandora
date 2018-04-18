
Before beginning to use Pandora the user will need a general perspective to the different components of an Agent-Based Model. If you have not developed any ABM before we recommend you to read some [related bibliography](LINK_A_BIBLIOGRAPHY) before starting to program. You should also know some basic concepts of Object Oriented programming, [the programming paradigm behind Pandora](TUTORIAL_OBJECT_ORIENTED)

In Pandora, the two main concepts that any ABM should model are:

- **World** -- For starters, any ABM must define an environment. Pandora calls it the World, and is the place where any entity of the system lives and interacts with other components of it. It can be as complex or simple as the user wants, going from an abstract spatial coordinates system to a realistic model with several layers of information, called raster maps. Following the convention of Geographical Information Systems, a raster map is a bidimensional matrix with a regularly distributed lattice of cells. Each cell will contain a value and a maximum value. A raster map can be static or dynamic, as Pandora allows these layers to be cellular automata, where structures can evolve in time.

- **Agent** -- This is the main actor of our system. An agent is a piece of software that models an entity of the world, with its own internal state (that can vary on time). An agent can have different properties, like a particular spatial position, decision-making processes and interactive capabilities. The traditional life of an agent is divided in regular time steps, where this cycle is executed:

	1. The agent gathers information about its surroundings
	2. Decision-making processes are explored, in order to choose which actions the agent wants to execute
	3. Chosen actions are executed, interacting with the environment and other agents, modifying the internal state, etc.

As we said before, any Pandora ABM will define a World, and usually one or several types of Agents. In order to do that a user can choose one of these approaches:

1. pyPandora. This ready-to-use scripting interface uses Python language to access Pandora utilities. It is strongly advised that first users begins with it, as it will allow to create our first ABMs with a few lines.
2. C++ Pandora Library. Advanced users can program ABMs in C++ programming language. This interface allows to access the full power of the library, including automatic generation of parallelized code (ready to be executed in HPC).

We recommend to start with [pyPandora tutorial](01_getting_started_pyPandora.md) and the introduction to the [pyAnalysis package](04_pyanalysis.md). C++ programmers and experienced users should follow the [C++ tutorial](02_getting_started_pandora.md),the [C++ Analysis package](05_analysis.md), the description of the [Parallel package](TUTORIAL_PARALLELIZATION), and the way Pandora models agents with advanced behaviors and [decision making processes](TUTORIAL_ADVANCED_AI). To verify simulations you will also need to understand [how the Logger system works](03_logging_system.md).
Finally, Pandora has several ways to interact with other software packages. Users can take a look to the tutorial on [GIS integration](TUTORIAL_GIS), and [statistical output](ANALYSIS_R).


[Next - install](00_installing.md)
Or [Next - install OSX](00_installing_osx.md)
