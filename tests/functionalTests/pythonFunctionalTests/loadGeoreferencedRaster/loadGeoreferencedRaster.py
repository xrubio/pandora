#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Simulation, Agent, World, Point2DInt, SizeInt, RectangleInt, SpacePartition, GeneralState, RasterLoader

class RasterWorld(World):
    _rasterFileName = 'no file'
    def __init__(self, simulation, scheduler, rasterFileName ):
        World.__init__( self, simulation, scheduler, False)
        self._rasterFileName = rasterFileName 

    def createRasters(self):
        self.registerDynamicRaster('raster', True)
        loader = GeneralState.rasterLoader()
        loader.fillGDALRaster(self.getDynamicRaster('raster'), self._rasterFileName)
        return

    def createAgents(self):
        return

def main():
    rasterFileName = '../../../resources/test.tiff'
    simulation = Simulation(SizeInt(120,120), 1)

    rasterWorld = RasterWorld(simulation, RasterWorld.useOpenMPSingleNode('data/raster.h5'), rasterFileName)
    rasterWorld.initialize()
    rasterWorld.run()

if __name__ == "__main__":
    main()


