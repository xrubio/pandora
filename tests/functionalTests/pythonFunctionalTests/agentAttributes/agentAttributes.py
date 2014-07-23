#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Config, Agent, World, Point2DInt, SizeInt, RectangleInt

class CustomAgent(Agent):
    stringAttribute = ''
    intAttribute = 0
    floatAttribute = 0.1

    def __init__(self, id):
        Agent.__init__( self, id)

    def registerAttributes(self):
        self.registerIntAttribute('int attr')
        self.registerFloatAttribute('float attr')
        self.registerStringAttribute('str attr')

    def updateState(self):
        return

    def serialize(self):
        self.serializeIntAttribute('int attr', self.intAttribute)
        self.serializeFloatAttribute('float attr', self.floatAttribute)
        self.serializeStringAttribute('str attr', self.stringAttribute)
        return

    def __str__(self):
        return 'agent: '+self.id+' pos: '+str(self.position)+' str attribute: '+self.stringAttribute +' int value: '+str(self.intAttribute) +' float value: '+str(self.floatAttribute)

class CustomWorld(World):
    def __init__(self, config, scheduler):
        World.__init__( self, config, scheduler, False)

    def createAgents(self):
        for i in range(0,100):
            newAgent = CustomAgent('CustomAgent_'+str(i))
            self.addAgent(newAgent)
            newAgent.setRandomPosition()
            newAgent.stringAttribute = 'string attribute: '+str(i)
            newAgent.intAttribute = i
            newAgent.floatAttribute = i/100.0

def main():
    config = Config(SizeInt(64,64), 5, "sequential/data.hdf5")

    customWorld = CustomWorld(config, CustomWorld.useOpenMPSingleNode())
    customWorld.initialize()
    customWorld.run()

if __name__ == "__main__":
    main()


