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
		size = self.getBoundaries()._size
		origin = self.getBoundaries()._origin
		for i in range(origin._x, origin._x+size._width):
			for j in range(origin._y, origin._y+size._height):
				pos = Point2DInt(i,j)
				raster.setValue(pos, 1+self.currentStep)

mySimulation = Simulation(SizeInt(64,64), 10)
myWorld = MyWorld(mySimulation)
myWorld.initialize()
myWorld.run()

