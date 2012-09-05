#!/usr/bin/python

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import Point2DInt, Simulation, World


class MyWorld(World):
	def __init__(self, simulation ):
		World.__init__( self, simulation)

	def createRasters(self):
		self.registerDynamicRaster("test", 1)
		self.getDynamicRaster("test").setInitValues(0, 10, 0)

	def createAgents(self):
		return

	def stepEnvironment(self):
		print 'executing step environment'
		raster = self.getDynamicRaster("test")
		size = raster.getSize()
		for i in range(0, size._x):
			for j in range(0, size._y):
				pos = Point2DInt(i,j)
				raster.setValue(pos, 1+self.currentStep)

mySimulation = Simulation(64, 10)
myWorld = MyWorld(mySimulation)
myWorld.initialize()
myWorld.run()

