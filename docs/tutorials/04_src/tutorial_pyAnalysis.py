#!/usr/bin/python

import sys
sys.path.append('../../../pandora/pyPandora/')
sys.path.append('../../../pandora/')
from pyPandora import SimulationRecord, AgentResults, AgentMean, AgentSum, AgentNum, AgentHDFtoSHP, RasterMean, RasterSum, RasterResults

record = SimulationRecord()
record.loadHDF5('data/results.h5', 1, 1)

agentResults = AgentResults(record, 'agents.csv', 'MyAgent', ';')
agentResults.addAnalysis(AgentNum())
agentResults.addAnalysis(AgentMean('x'))
agentResults.addAnalysis(AgentMean('y'))
agentResults.addAnalysis(AgentMean('resources'))
agentResults.addAnalysis(AgentSum('resources'))
agentResults.addAnalysis(AgentHDFtoSHP('shp/agents.shp', -1))

agentResults.compute()

rasterResults = RasterResults(record, 'rasters.csv', 'resources', ';')
rasterResults.addAnalysis(RasterMean())
rasterResults.addAnalysis(RasterSum())

rasterResults.compute()

