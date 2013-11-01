#!/usr/bin/python

import sys, random
sys.path.append('../../../pyPandora/')
sys.path.append('../../../')
from pyPandora import SimulationRecord, GlobalAgentStats, AgentNum, AgentMean, AgentSum, AgentStdDev, GlobalRasterStats, RasterMean, RasterSum

record = SimulationRecord(1, False)
record.loadHDF5('../data/results.h5', True, True)

agentResults = GlobalAgentStats(';')

agentResults.addAnalysis(AgentNum())
agentResults.addAnalysis(AgentMean('strength x100'))
agentResults.addAnalysis(AgentMean('aggressiveness x100'))
agentResults.addAnalysis(AgentStdDev('strength x100'))
agentResults.addAnalysis(AgentStdDev('aggressiveness x100'))
agentResults.addAnalysis(AgentSum('strength x100'))
agentResults.addAnalysis(AgentSum('aggressiveness x100'))

agentResults.applyTo(record,'farmers.csv', 'Farmer')
agentResults.applyTo(record,'herders.csv', 'Herder')

rasterResults = GlobalRasterStats(';')
rasterResults.addAnalysis(RasterMean())
rasterResults.addAnalysis(RasterSum())

rasterResults.applyTo(record, 'conflicts.csv', 'conflicts')

