#!/usr/bin/python

import sys, random
sys.path.append('../../../pandora/pyPandora/')
sys.path.append('../../../pandora/')
from pyPandora import Simulation, World, Agent, Point2DInt

class MyAgent(Agent):
	gatheredResources = 0

	def __init__(self, id):
		Agent.__init__( self, id)
		print 'constructing agent: ' + self.id
	
	def updateState(self):
		print 'updating state of: ' + self.id
		newPosition = self.position
		newPosition._x = newPosition._x + random.randint(-1,1)
		newPosition._y = newPosition._y + random.randint(-1,1)
	
		if self.getWorld().checkPosition(newPosition):		
			self.position = newPosition
			self.gatheredResources = self.gatheredResources + self.getWorld().getValue('resources', self.position)
			self.getWorld().setValue('resources', self.position, 0)			

	def registerAttributes(self):
		self.registerIntAttribute('resources')

	def serialize(self):
		print 'serializing MyAgent: ' + self.id
		self.serializeIntAttribute('resources', self.gatheredResources)

class MyWorld(World):
	def __init__(self, simulation ):
		World.__init__( self, simulation)
		print 'constructing MyWorld'
	
	def createRasters(self):
		print 'creating rasters'
		self.registerDynamicRaster("resources", 1)
		self.getDynamicRaster("resources").setInitValues(0, 10, 0)
		return

	def createAgents(self):	
		print 'creating agents'
		for i in range (0, 10):
			newAgent = MyAgent('MyAgent_'+str(i))
			self.addAgent(newAgent)
			newAgent.setRandomPosition()

def main():
	print 'getting started with pyPandora'
	numTimeSteps = 10
	worldSize = 64
	mySimulation = Simulation(worldSize, numTimeSteps)
	myWorld = MyWorld(mySimulation)
	myWorld.initialize()
	myWorld.run()
	print 'simulation finished'

if __name__ == "__main__":
    main()

