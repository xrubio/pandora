#!/usr/bin/python3

# unit testing of pyPandora

import sys, random
import unittest
sys.path.append('../')
sys.path.append('../../')

from pyPandora import Simulation, Agent, World, Point2DInt

class TestAgent(Agent):
	def __init__(self, id):
		Agent.__init__( self, id)

	def updateState(self):
		print(self,'executed')
		return

	def serialize(self):
		return

class TestWorld(World):
	def __init__(self, simulation ):
		World.__init__( self, simulation)

	def createRasters(self):
		return

	def createAgents(self):
		return

class TestPyPandora(unittest.TestCase):
	
	def setUp(self):
		return
	
	def testEqualityPoint(self):
		point1 = Point2DInt(4,5)
		point2 = Point2DInt(2,5)
	
		self.assertNotEqual(point1, point2)

		point2._x = 4
		self.assertEqual(point1, point2)

		point2._y = 100
		self.assertNotEqual(point1, point2)

	def testSimulationSize(self):
		mySimulation = Simulation(Point2DInt(10,10), 1)
		size = Point2DInt(9,10)

		self.assertNotEqual(mySimulation.size, size)
		size._x = 10
		self.assertEqual(mySimulation.size, size)

	def testAgentRemovedIsNotExecuted(self):
		mySimulation = Simulation(Point2DInt(10,10), 1)
		myWorld = TestWorld(mySimulation)
		myWorld.initialize()

		myAgent = TestAgent('agent_0')
		myWorld.addAgent(myAgent)
		myAgent.setRandomPosition()
		self.assertEqual(myAgent.exists, True)
		myAgent.remove()
		self.assertEqual(myAgent.exists, False)
		myWorld.run()
       
if __name__ == '__main__':
    unittest.main()
