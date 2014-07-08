#!/usr/bin/python3

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import Point2DInt, Simulation, World, SizeInt


class MyWorld(World):
	def __init__(self, simulation ):
		World.__init__( self, simulation)

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

mySimulation = Simulation(SizeInt(64,64), 10)
myWorld = MyWorld(mySimulation)
myWorld.initialize()
myWorld.run()

