#!/usr/bin/python

import sys
sys.path.append('..')
sys.path.append('../../')
from pyPandora import SimulationRecord, AgentResults, AgentMean, AgentSum, AgentNum, AgentHDFtoSHP, RasterMean, RasterSum, RasterResults

record = SimulationRecord()
record.loadHDF5('data_test/gujarat.h5', 1, 1)

agentResults = AgentResults(record, 'agents.csv', 'HunterGatherer', ';')
agentResults.addAnalysis(AgentNum())
agentResults.addAnalysis(AgentMean('children'))
agentResults.addAnalysis(AgentSum('children'))
agentResults.addAnalysis(AgentMean('collected resources'))
agentResults.addAnalysis(AgentHDFtoSHP('shp/agents.shp', -1))

agentResults.compute()

rasterResults = RasterResults(record, 'resources.csv', 'resources', ';')
rasterResults.addAnalysis(RasterMean())
rasterResults.addAnalysis(RasterSum())

rasterResults.compute()

