#!/usr/bin/python

import sys
sys.path.append('../')
import libpyPandora

class Point2DInt(libpyPandora.Point2DIntStub):
	def __init__(self, x, y):
		libpyPandora.Point2DIntStub.__init__(self, x, y)

class StaticRaster(libpyPandora.StaticRasterStub):
	def __init__(self):
		libpyPandora.StaticRasterStub.__init__(self)

class Raster(libpyPandora.RasterStub):
	def __init__(self):
		libpyPandora.RasterStub.__init__(self)

class Simulation(libpyPandora.SimulationStub):
	def __init__(self, size, numSteps):
		libpyPandora.SimulationStub.__init__(self, size, numSteps )

class Agent(libpyPandora.AgentStub):
	def __init__(self, id):
		libpyPandora.AgentStub.__init__( self, id)

class World(libpyPandora.WorldStub):
	_listOfAgents = []
	def __init__(self, simulation ):
		libpyPandora.WorldStub.__init__( self, simulation, 0, 1, "data/results.h5")

	def addAgent(self, agent):
		self.addAgentStub(agent)
		self._listOfAgents.append(agent)

class SimulationRecord(libpyPandora.SimulationRecordStub):
	def __init__(self):
		libpyPandora.SimulationRecordStub.__init__( self )

class AgentMean(libpyPandora.AgentMeanStub):
	def __init__(self, attributeName):
		libpyPandora.AgentMeanStub.__init__( self, attributeName )

class AgentSum(libpyPandora.AgentSumStub):
	def __init__(self, attributeName):
		libpyPandora.AgentSumStub.__init__( self, attributeName )

class AgentNum(libpyPandora.AgentNumStub):
	def __init__(self):
		libpyPandora.AgentNumStub.__init__( self )

class AgentHDFtoSHP(libpyPandora.AgentHDFtoSHPStub):
	def __init__(self, fileName, numStep ):
		libpyPandora.AgentHDFtoSHPStub.__init__( self, fileName, numStep )

class RasterSum(libpyPandora.RasterSumStub):
	def __init__(self):
		libpyPandora.RasterSumStub.__init__( self )

class RasterMean(libpyPandora.RasterMeanStub):
	def __init__(self):
		libpyPandora.RasterMeanStub.__init__( self )

class AgentResults(libpyPandora.AgentResultsStub):	
	def __init__(self, simRecord, outputFile, typeName, separator ):
		libpyPandora.AgentResultsStub.__init__( self, simRecord, outputFile, typeName, separator )

class RasterResults(libpyPandora.RasterResultsStub):	
	def __init__(self, simRecord, outputFile, typeName, separator ):
		libpyPandora.RasterResultsStub.__init__( self, simRecord, outputFile, typeName, separator )

