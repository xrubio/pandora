#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Point2DInt, Config, World, SizeInt

class MyWorld(World):
	def __init__(self, config):
		World.__init__( self, config)

	def createRasters(self):
		self.registerDynamicRaster("test", 1)
		self.getDynamicRaster("test").setInitValues(0, 10, 0)

	def createAgents(self):
		return

	def stepEnvironment(self):
		print('executing step environment')
		raster = self.getDynamicRaster("test")
		for i in range(self.getBoundaries().left, self.getBoundaries().right+1):
			for j in range(self.getBoundaries().top, self.getBoundaries().bottom+1):
				pos = Point2DInt(i,j)
				raster.setValue(pos, 1+self.currentStep)

myConfig = Config(SizeInt(64,64), 10)
myWorld = MyWorld(myConfig)
myWorld.initialize()
myWorld.run()

