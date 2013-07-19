#!/usr/bin/python

import sys, random
sys.path.append('../../../pyPandora/')
sys.path.append('../../../')
from pyPandora import SimulationRecord, AgentResults, AgentMean, AgentSum, AgentNum, AgentHDFtoSHP, RasterMean, RasterSum, RasterResults

record = SimulationRecord()
record.loadHDF5('../data/results.h5', 1, 1)

agentResults = AgentResults(record, 'agents.csv', 'Farmer', ';')
agentResults.addAnalysis(AgentNum())

agentResults.apply()

#rasterResults = RasterResults(record, 'resources.csv', 'test', ';')
#rasterResults.addAnalysis(RasterMean())
#rasterResults.addAnalysis(RasterSum())

#rasterResults.apply()

