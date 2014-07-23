#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Config, Agent, World, Point2DInt, SizeInt, RectangleInt, SpacePartition, GeneralState, RasterLoader

class RasterWorld(World):
    _rasterFileName = 'no file'
    def __init__(self, config, scheduler, rasterFileName ):
        World.__init__( self, config, scheduler, False)
        self._rasterFileName = rasterFileName 

    def createRasters(self):
        self.registerStaticRaster('raster', True)
        loader = GeneralState.rasterLoader()
        loader.fillGDALRaster(self.getStaticRaster('raster'), self._rasterFileName)
        return

    def createAgents(self):
        return

def main():
    rasterFileName = '../../../resources/rasterWithColor.tiff'

    rasterWorld = RasterWorld(Config(SizeInt(708,708), 1), RasterWorld.useOpenMPSingleNode(), rasterFileName)
    rasterWorld.initialize()
    rasterWorld.run()

if __name__ == "__main__":
    main()


