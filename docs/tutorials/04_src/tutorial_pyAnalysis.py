#!/usr/bin/python3

import sys
pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')
from pyPandora import SimulationRecord, GlobalAgentStats, AgentMean, AgentSum, AgentNum, RasterMean, RasterSum, GlobalRasterStats

record = SimulationRecord()
record.loadHDF5('data/results.h5', True, True)

agentResults = GlobalAgentStats(';')
agentResults.addAnalysis(AgentNum())
agentResults.addAnalysis(AgentMean('x'))
agentResults.addAnalysis(AgentMean('y'))
agentResults.addAnalysis(AgentMean('resources'))
agentResults.addAnalysis(AgentSum('resources'))

agentResults.applyTo(record, 'agents.csv', 'MyAgent')

rasterResults = GlobalRasterStats(';')
rasterResults.addAnalysis(RasterMean())
rasterResults.addAnalysis(RasterSum())
rasterResults.applyTo(record, 'rasters.csv', 'resources')

