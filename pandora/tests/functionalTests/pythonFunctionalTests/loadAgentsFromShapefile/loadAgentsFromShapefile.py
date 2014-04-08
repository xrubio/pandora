#!/usr/bin/python3

import sys, random

pandoraDir = '/home/xrubio/workspace/pandoraMaster/'

sys.path.append(pandoraDir+'/pandora/')
sys.path.append(pandoraDir+'/pandora/pyPandora/')

from pyPandora import Simulation, Agent, World, Point2DInt, SizeInt, RectangleInt, SpacePartition

class ShpAgent(Agent):
	def __init__(self, id):
		Agent.__init__( self, id)

	def updateState(self):
		return

	def serialize(self):
		return

class ShpWorld(World):
    _shpFileName = 'no file'
    def __init__(self, simulation, scheduler, shpFileName ):
        World.__init__( self, simulation, scheduler, False)
        self._shpFileName = shpFileName

    def createRasters(self):
        return

    def createAgents(self):
        loader = self.getLoader()
        loader.openFile(self._shpFileName)
        loader.setActiveLayer(0)
        for i in range(0, loader.getNumFeatures()):
            print('creating agent:',loader.getFieldAsString(i,'name'))
        #newAgent = ShpAgent('ShpAgent_',loader.getFieldAsString(i, 'name'))


def main():
    shpFileName = 'resources/test.shp'
    simulation = Simulation(SizeInt(64,64), 1)

    shpWorld = ShpWorld(simulation, ShpWorld.useOpenMPSingleNode('data/shp.h5'), shpFileName)
    shpWorld.initialize()
    shpWorld.run()

if __name__ == "__main__":
    main()


