#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

import random

from pyPandora import Config, Agent, World, Point2DInt, SizeInt

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
	def __init__(self, config):
		World.__init__( self, config)

	def createRasters(self):		
		self.registerDynamicRaster("test", 1)
		self.getDynamicRaster("test").setInitValues(0, 0, 0)

	def createAgents(self):
		for i in range (0, 300):
			newAgent = MyAgent('id_'+str(i))
			self.addAgent(newAgent)
			newAgent.setRandomPosition()

myConfig = Config(SizeInt(32,32), 300)
myWorld = MyWorld(myConfig)
myWorld.initialize()
myWorld.run()

