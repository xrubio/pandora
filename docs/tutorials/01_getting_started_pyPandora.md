
Getting started with pyPandora

To begin this tutorial you will need to have Python and Pandora installed on your computer. Take a look to the Installation (link INSTALLING PANDORA) documentation if you have not make it before.

We will create a simple Agent-Based Model with a basic World and a type of agents. These agents will randomly move through the World, and collect resources from the environment.

1) Create a python scripting file. It is recommended to have some basic background on Python programming language (LINK PYTHON).
First of all, create an empty text file 'tutorial_pandora_01.py' with this header:

#!/usr/bin/python3

This is an execution file, so you will need to grant it execution privileges, from the command console:

chmod +x tutorial_pyPandora.py

02) Create a 'main' function

Next step is to create a 'main' function. This is the code that will be executed once we invoke the python interpreter, and the starting point for our model. Add these lines to your file:

def main():
    print('getting started with pyPandora')
    print('simulation finished')

if __name__ == "__main__":
    main()

Finally, save the file and execute it from command line:
./tutorial_pyPandora.py

You should see, as an output, these lines:

getting started with pyPandora
simulation finished

If this is not the case something went wrong. Please check your Python code and the execution privileges.

3) Create MyWorld

Now that Python is working we need to create our first World. We will create a class MyWorld, that inherits its functionality from a basic class called World provided by the library. MyWorld will only have a field of resources (called, obviously, 'resources').

Start defining an object MyWorld, child of World:

class MyWorld(World):
    def __init__(self, config):
        World.__init__( self, config)
        print('constructing MyWorld')
    
    def createRasters(self):
        print('creating rasters')
        return

    def createAgents(self):
        print('creating agents')
        return

As you can see, we have defined 3 methods:
    1) __init__ is the constructor method of our class. World needs a Config object where basic execution data will be stored (number of time steps, size of World), so we pass it to World's constructor. Two additional pure virtual methods must be defined:
    2) createRasters is called at initialization time. Here we will define the different components of MyWorld that must be present once the execution begins.
    3) createAgents is also called at init time. It is the method where initial Agents are created and configured.

Finally, we need to create and configure an instance of MyWorld inside the main method, that now will have this code:

def main():    
    print('getting started with pyPandora')
    numTimeSteps = 10
    worldSize = SizeInt(64,64)
    myConfig = Config(worldSize, numTimeSteps)
    myWorld = MyWorld(myConfig)
    print('simulation finished')

We create an instance of Config class called myConfig. It contains two parameters needed to execute our model: the number of time steps that the simulation will last, and the size of our world. myConfig is passed to the constructor of MyWorld, and that's it.
Let's try to execute this code. It will throw a critical error!

Traceback (most recent call last):
  File "./tutorial_pyPandora.py", line 3, in <module>
    class MyWorld(World):
NameError: name 'World' is not defined

In order to find World and Simulation, our code must define the place where these classes were defined. We must add these directories to the list of paths defined in Python, at the beginning of our file:

#!/usr/bin/python3

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import Config, World, SizeInt

After adding these lines, the execution of tutorial_pyPandora.py should throw an output like this:

getting started with pyPandora
constructing MyWorld
simulation finished

MyWorld is constructed, but the model is not executed. To achieve this, we have to add these lines to our main method:

    myWorld.initialize()
    myWorld.run()

The first line initialized the simulation (creating the objects defined in createRasters and createAgents). The main method will now look like:

def main():
    print('getting started with pyPandora')
    numTimeSteps = 10
    worldSize = SizeInt(64,64)
    myConfig = Config(worldSize, numTimeSteps)
    myWorld = MyWorld(myConfig)
    myWorld.initialize()
    myWorld.run()
    print('simulation finished')

And the output:

getting started with pyPandora
constructing MyWorld
simulation: 0 of: 1 initialized
creating rasters
 id: 0 size: 64 num tasks: 1 and steps: 10
creating agents
simulation finished

We can check that pyPandora has called our two init methods (createRasters and createAgents), and 1 task (with id=0) was created, that executed MyWorld for 10 time steps. If we take a closer look to the directory where tutorial_pyPandora.py is, we will see that a new 'data' directory has been created, containing to files: 'agents-0.abm' and 'results.h5'. You can view the results of the simulation using Cassandra application.
Open cassandra, and go to File->Select Simulation. Look for the directory where you are working, and select 'results.h5' from 'data' folder. You will see a small square of 64x64 points, and a few more things, nothing spectacular. Anyway, it is the proof that your code works and is generating something, congratulations!

4) Create some rasters

Now is time to fill MyWorld. Go to createRasters method, and add these lines:
    
self.registerDynamicRaster("resources", 1)
self.getDynamicRaster("resources").setInitValues(0, 10, 0)

The first line tells World that it needs to register a new dynamic raster map (it can be modified). The first parameter states that this raster map is called resources, and the second one that it must be stored in 'results.h5' file.
The second line sets the initialized values of the entire raster map. The first two parameters are the minimum and maximum value for the entire world, and the third one is the default init value.

If you execute again 'tutorial_pyPandora.py', and check 'results.h5' with cassandra you will realize that a 'resources' raster map has been created. Try to 'play' the simulation; the default behavior for a raster map is to increate the value of each cell 1 unit until reaching the maximum value, so if you check the step number 9 you can check that every cell has a value of '9'. (LINK A figure_01.png)

5) Define MyAgent

Now that MyWorld is working we will need to define our own MyAgent. Start defining MyAgent as a child class of Agent:
    
class MyAgent(Agent):
    def __init__(self, id):
        Agent.__init__( self, id)
        print('constructing agent: ',self.id)
    
    def updateState(self):
        print('updating state of: ',self.id)
        return
    
    def serialize(self):
        print('serializing MyAgent: ',self.id)
        return

This class will have three methods:
    1) __init__ - The constructor, just like we did with MyWorld. It only gets one parameter, a string that will be used as the 'id' of the agent. This attribute is very important: every instance of Agent must have a unique id. If this is not the case, Pandora will be confused about which agent is every instance.
    2) updateState - This method is the core of a simple Pandora Agent. It is executed every time step, and is the place where the internal state of Agent is modified.
    3) serialize - In order to analyze the results, we will need to store part of the internal state of an Agent every time step. This is done in this method, but it will be empty by now. By default, Pandora always serializes the id and position of an agent so we don't need to do that. Additionally, a variable called 'exists' that checks whether this agent is alive.

If we execute this code we will get an error saying that Agent is not found. We have to add it to the classes included in our file:

from pyPandora import Config, World, Agent, SizeInt 

6) Define behavior

We will write some basic code inside updateState to modify its state. For now we will basically move our Agents randomly through the space defined in MyWorld:

    def updateState(self):
        print('updating state of: ',self.id)
        newPosition = self.position
        newPosition._x = newPosition._x + random.randint(-1,1)
        newPosition._y = newPosition._y + random.randint(-1,1)

        if self.getWorld().checkPosition(newPosition):        
            self.position = newPosition

You can see that we have created a local variable called 'newPosition'. It is initialized to the current location of MyAgent (self.position), but we modify randomly these values with a value going of -1, 0 or 1. Finally, we check if the position is correct in terms of MyWorld (to ensure that it is inside boundaries), and we update self.position. It is worth to note that newPosition is an instance of class Point2DInt.

We also have to import random functionaly from Python core libraries:

import sys, random

7) Creating agents
We have everything ready for our first Agent-Based Model, but we have to create some instances of our brand new little MyAgent class. As stated before, we will add them in the 'createAgents' method of MyWorld:

    def createAgents(self):
        print('creating agents')
        for i in range (0, 10):
            newAgent = MyAgent('MyAgent_'+str(i))
            self.addAgent(newAgent)
            newAgent.setRandomPosition()

Pretty straightforward; with these lines we are creating 10 agents (called MyAgent_0, MyAgent_1, until MyAgent_9), adding them to the World, and setting initial positions for all of them. 
Everything ready! If we execute again tutorial_pyPandora.py we will get a list of messages communicating us that 10 agents have been created, and a sequence of 'updateState' and 'serialize' methods have been executed. If we check 'results.h5' with cassandra we should see something like Figure 2 (LINK figure_02.png)

8) Gathering resources
To finish this tutorial we will modify MyAgent to gather 'resources' from MyWorld, and serialize this value. We go again to the 'updateState' method and add these lines:
    
    def updateState(self):
        print('updating state of: ',self.id)
        newPosition = self.position
        newPosition._x = newPosition._x + random.randint(-1,1)
        newPosition._y = newPosition._y + random.randint(-1,1)
        
        if self.getWorld().checkPosition(newPosition):        
            self.position = newPosition
            self.gatheredResources = self.gatheredResources + self.getWorld().getValue('resources', self.position)
            self.getWorld().setValue('resources', self.position, 0)            

First of all, we will get the value of 'resources' from MyWorld, in the cell we recently moved. This is added to gatheredResources, an attribute where the total value of resources gathered during the simulation is stored. This cell is set to 0 in raster map 'resources' of MyWorld
We also have to define this gatheredResources attribute:


class MyAgent(Agent):
    def __init__(self, id):
        Agent.__init__( self, id)
        self.gatheredResources = 0
        print('constructing agent: ',self.id)



And store the value of gatheredResources  in serialize method:

    def serialize(self):
        print('serializing MyAgent: ',self.id)
        self.serializeIntAttribute('resources', self.gatheredResources)

To end the tutorial, an additional method needs to be defined in MyAgent, called registerAttributes. This method, called in initialization time, indicates Pandora which attributes will be stored during execution:

    def registerAttributes(self):
        self.registerIntAttribute('resources')

You can execute the simulation, and check that agents collect resources from the world and accumulate them from time step to time step, as can be seen in figure 3 (LINK figure_03.png)

Congratulations, you have created your first Agent-Based Model using pyPandora!

[Next - Log System](03_logging_system.md)
