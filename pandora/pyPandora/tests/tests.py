#!/usr/bin/python

# unit testing of pyPandora

import sys, random
import unittest
sys.path.append('../')
sys.path.append('../../')

from pyPandora import Simulation, Agent, World, Point2DInt

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
       
if __name__ == '__main__':
    unittest.main()
