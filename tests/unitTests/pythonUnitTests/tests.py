#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

import unittest

from pyPandora import Config, Agent, World, Point2DInt, SizeInt, RectangleInt, SpacePartition, ShpLoader, RasterLoader, GeneralState, StaticRaster, DynamicRaster

class TestAgent(Agent):
    def __init__(self, id):
        Agent.__init__( self, id)

    def updateState(self):
        return

    def serialize(self):
        return
    
class TestWorld(World):
    def __init__(self, config, scheduler):
        World.__init__( self, config, scheduler, False)

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
        myConfig = Config(SizeInt(10,10), 1)
        size = SizeInt(9,10)

        self.assertNotEqual(myConfig.size, size)
        size._width = 10
        self.assertEqual(myConfig.size, size)
    def testAgentRemovedIsNotExecuted(self):
        myConfig = Config(SizeInt(10,10), 1)
        myWorld = TestWorld(myConfig, TestWorld.useSpacePartition(1, False))
        myWorld.initialize()

        myAgent = TestAgent('agent_0')
        myWorld.addAgent(myAgent)
        myAgent.setRandomPosition()
        self.assertEqual(myAgent.exists, True)
        myAgent.remove()
        self.assertEqual(myAgent.exists, False)
        myWorld.run()

    def testExecuteTwoWorlds(self):
        myConfig = Config(SizeInt(10,10), 1)
        myWorld = TestWorld(myConfig, TestWorld.useSpacePartition(1, False))
        myWorld.initialize()
        myWorld.run()
        myWorld = TestWorld(myConfig, TestWorld.useSpacePartition(1, False))
        myWorld.initialize()
        myWorld.run()
    
    def testAgentRemovedIsNotInsideNeighbours(self):
        myConfig = Config(SizeInt(10,10), 1)
        myWorld = TestWorld(myConfig, TestWorld.useSpacePartition(1, False))
        myWorld.initialize()
        myWorld.run()

        myAgent0 = TestAgent('agent_0')
        myAgent1 = TestAgent('agent_1')
        myAgent2 = TestAgent('agent_2')
        myWorld.addAgent(myAgent0)
        myWorld.addAgent(myAgent1)
        myWorld.addAgent(myAgent2)
        myAgent0.setRandomPosition()
        myAgent1.setRandomPosition()
        myAgent2.setRandomPosition()

        agentIds = myWorld.getNeighboursIds(myAgent0, 20, 'agent')
        self.assertEqual(len(agentIds), 2)
        myAgent1.remove()
        myAgent2.remove()
        agentIds = myWorld.getNeighboursIds(myAgent0, 20, 'agent')
        self.assertEqual(len(agentIds), 0)
        
    def testRectangleEquals(self):
        aRectangle = RectangleInt.fromCoordinates(5,10,20,30);
        bRectangle = RectangleInt(SizeInt(1,1), Point2DInt(3,7))

        self.assertNotEqual(aRectangle, bRectangle)

        bRectangle._origin = Point2DInt(5,10)
        self.assertNotEqual(aRectangle, bRectangle)

        bRectangle._size = SizeInt(16,21)
        self.assertEqual(aRectangle, bRectangle)

    def testRectangleContainsPoint(self):
        aRectangle = RectangleInt.fromCoordinates(0,0,20,10);
        aPoint = Point2DInt(-1,-1)
        
        self.assertFalse(aRectangle.contains(aPoint))

        aPoint._x = 0       
        self.assertFalse(aRectangle.contains(aPoint))

        aPoint._y = 0
        self.assertTrue(aRectangle.contains(aPoint))

        aPoint._x = 20
        self.assertTrue(aRectangle.contains(aPoint))

        aPoint._y = 10
        self.assertTrue(aRectangle.contains(aPoint))

        aPoint._x = 21
        self.assertFalse(aRectangle.contains(aPoint))

    def testLoadShapefile(self):
        loader = GeneralState.shpLoader()
        loader.open('../../resources/test.shp')

        self.assertEqual(4,loader.getNumFeatures())
        self.assertEqual(Point2DInt(32,22), loader.getPosition(3))
        self.assertEqual('label d', loader.getFieldAsString(3, 'label'))
        self.assertEqual(4, loader.getFieldAsInt(3, 'intValue'))
        self.assertAlmostEqual(4.5, loader.getFieldAsFloat(3, 'floatValue'))

    def testGetUnknownRasterThrowsException(self):
        myConfig = Config(SizeInt(10,10), 1)
        myWorld = TestWorld(myConfig, TestWorld.useSpacePartition(1, True))
        myWorld.initialize()
        myWorld.run()
        try:
            myWorld.getDynamicRaster('unknown')
            self.assertEqual(1!=2)
        except:
            dummy = 1

        try:
            myWorld.getStaticRaster('unknown')
            self.assertEqual(1!=2)
        except:
            dummy = 1

        try:
            myWorld.getStaticRaster(1000)
            self.assertEqual(1!=2)
        except:
            dummy = 1

        try:
            myWorld.getDynamicRaster(1000)
            self.assertEqual(1!=2)
        except:
            dummy = 1

    def testLoadStaticRaster(self):
        aRaster = StaticRaster()
        loader = GeneralState.rasterLoader()
        loader.fillGDALRaster(aRaster, '../../resources/test.tiff')
        self.assertEqual(120, aRaster.getSize()._width)
        self.assertEqual(120, aRaster.getSize()._height)
        self.assertEqual(139, aRaster.getValue(Point2DInt(39,39)))
   
    def testLoadDynamicRaster(self):
        aRaster = DynamicRaster()
        loader = GeneralState.rasterLoader()
        loader.fillGDALRaster(aRaster, '../../resources/test.tiff')
        self.assertEqual(120, aRaster.getSize()._width)
        self.assertEqual(120, aRaster.getSize()._height)
        self.assertEqual(139, aRaster.getValue(Point2DInt(39,39)))
    
    def testAddAgent(self):
        myConfig = Config(SizeInt(10,10), 1)
        myWorld = TestWorld(myConfig, TestWorld.useSpacePartition(1, False))
        myWorld.initialize()

        myAgent = TestAgent('agent_0')
        myWorld.addAgent(myAgent)
        myAgent.setRandomPosition()
        myWorld.run()
        self.assertEqual(myAgent.getWorld(), myWorld)

if __name__ == '__main__':
    unittest.main()

