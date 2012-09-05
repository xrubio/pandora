#!/usr/bin/python

import sys, random
sys.path.append('../')
sys.path.append('../../')

from pyPandora import Simulation, Agent, World, Point2DInt

class MyAgent(Agent):
	def __init__(self, id):
		Agent.__init__( self, id)

	def updateState(self):
		newPosition = Point2DInt(self.position._x + random.randint(-1,1), self.position._y + random.randint(-1,1))
		if self.getWorld().checkPosition(newPosition):
			self.position = newPosition

	def serialize(self):
		return

class MyWorld(World):
	def __init__(self, simulation ):
		World.__init__( self, simulation)

	def createRasters(self):		
		self.registerDynamicRaster("test", 1)
		self.getDynamicRaster("test").setInitValues(0, 0, 0)

	def createAgents(self):
		for i in range (0, 10):
			newAgent = MyAgent('id_'+str(i))
			self.addAgent(newAgent)
			newAgent.setRandomPosition()

mySimulation = Simulation(32, 10)
myWorld = MyWorld(mySimulation)
myWorld.initialize()
myWorld.run()

