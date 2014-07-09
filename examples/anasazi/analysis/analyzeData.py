#!/usr/bin/python3

import sys, random
sys.path.append('/home/xrubio/workspace/pandora/pandora/pyPandora/')
sys.path.append('/home/xrubio/workspace/pandora/pandora/')
from pyPandora import Simulation, Agent, World, Point2DInt, SimulationRecord, GlobalAgentStats, AgentNum, AgentMean, AgentSum, AgentStdDev, GlobalRasterStats, RasterMean, RasterSum

record = SimulationRecord(1, False)
record.loadHDF5('../data/anazasi.h5', True, True)

agentResults = GlobalAgentStats(';')
agentResults.addAnalysis(AgentNum())
#agentResults.addAnalysis(AgentMean('estimates'))
#agentResults.addAnalysis(AgentStdDev('strength x100'))
#agentResults.addAnalysis(AgentSum('strength x100'))
agentResults.applyTo(record,'households.csv', 'Household')

#rasterResults = GlobalRasterStats(';')
#rasterResults.addAnalysis(RasterMean())
#rasterResults.addAnalysis(RasterSum())

#rasterResults.applyTo(record, 'water.csv', 'yield x100')

