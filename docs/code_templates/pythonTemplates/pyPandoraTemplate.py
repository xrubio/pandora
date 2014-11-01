#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

import random

from pyPandora import Config, Agent, World, Point2DInt, SizeInt

class HelloAgent(Agent):
    def __init__(self, id):
        """ constructor of class HelloAgent """
        Agent.__init__(self, id)
        self._value = 1

    def updateState(self):
        print(self,'updating state')

    def registerAttributes(self):
        print(self,'registering attributes to be serialized')
        self.registerIntAttribute('value')

    def serialize(self):
        print(self,'serializing')
        self.serializeIntAttribute('value', self._value)
        return

class HelloWorld(World):
    def __init__(self, config):
        """ constructor of class HelloWorld """
        World.__init__( self, config)

    def createRasters(self):
        """ init function where rasters are created """
        print(self,'creating rasters')
        self.registerDynamicRaster("values", True)
        self.getDynamicRaster("values").setInitValues(0, 0, 0)

    def createAgents(self):
        """ init function where agents are created """
        print(self,'creating agents')
        newAgent = HelloAgent('HelloAgent_0')
        self.addAgent(newAgent)
        newAgent.setRandomPosition()

    def stepEnvironment(self):
        print('step of environment')


def main():
    """ this file provides a template that can be used to create new pyPandora models """

    size = SizeInt(10,10)
    numSteps = 10
    myConfig = Config(size, numSteps)
    world = HelloWorld(myConfig)
    world.initialize()
    world.run()

if __name__ == "__main__":
    main()  

