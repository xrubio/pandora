#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Config, Agent, World, Point2DInt, SizeInt, RectangleInt, SpacePartition, GeneralState, ShpLoader

class ShpAgent(Agent):
    _label = ''
    _intValue = 0
    _floatValue = 0.1
    def __init__(self, id):
        Agent.__init__( self, id)

    def registerAttributes(self):
        self.registerIntAttribute('intValue')
        self.registerIntAttribute('floatValue')
        self.registerStringAttribute('label')

    def updateState(self):
        return

    def serialize(self):
        self.serializeIntAttribute('intValue', self._intValue)
        self.serializeIntAttribute('floatValue', int(self._floatValue))
        self.serializeStringAttribute('label', self._label)
        return

    def __str__(self):
        return 'agent: '+self.id+' pos: '+str(self.position)+' label: '+self._label+' int value: '+str(self._intValue)+' float value: '+str(self._floatValue)

class ShpWorld(World):
    _shpFileName = 'no file'
    def __init__(self, config, scheduler, shpFileName ):
        World.__init__( self, config, scheduler, False)
        self._shpFileName = shpFileName

    def createRasters(self):
        return

    def createAgents(self):
        loader = GeneralState.shpLoader()
        loader.open(self._shpFileName)
        for i in range(0, loader.getNumFeatures()):
            newAgent = ShpAgent('ShpAgent_'+loader.getFieldAsString(i, 'name'))
            position = loader.getPosition(i)
            position._y = self.getBoundaries()._size._height - position._y
            if not self.getBoundaries().contains(position):
                continue
            newAgent.position = position
            newAgent._label = loader.getFieldAsString(i, 'label')
            newAgent._intValue = loader.getFieldAsInt(i, 'intValue')
            newAgent._floatValue = loader.getFieldAsFloat(i, 'floatValue')
            print('loading agent num:',i,'-',newAgent)
            self.addAgent(newAgent)

def main():
    shpFileName = '../../../resources/test.shp'
    config = Config(SizeInt(64,64))

    shpWorld = ShpWorld(config, ShpWorld.useOpenMPSingleNode(), shpFileName)
    shpWorld.initialize()
    shpWorld.run()

if __name__ == "__main__":
    main()


