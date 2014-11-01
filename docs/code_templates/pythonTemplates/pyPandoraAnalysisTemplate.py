#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import SimulationRecord, GlobalAgentStats, AgentNum, AgentMean, AgentSum, AgentStdDev, GlobalRasterStats, RasterMean, RasterSum, SizeInt

def main():
    """ this file provides a template that can be used to analyze any Pandora model """
    record = SimulationRecord(1, False)
    record.loadHDF5('data/results.h5', True, True)

    agentResults = GlobalAgentStats(';')
    agentResults.addAnalysis(AgentNum())
    agentResults.addAnalysis(AgentMean('x'))
    agentResults.addAnalysis(AgentMean('y'))
    agentResults.addAnalysis(AgentMean('value'))
    agentResults.addAnalysis(AgentSum('value'))
    agentResults.applyTo(record, 'agents.csv', 'HelloAgent')

    rasterResults = GlobalRasterStats(';')
    rasterResults.addAnalysis(RasterMean())
    rasterResults.addAnalysis(RasterSum())
    rasterResults.applyTo(record, 'values.csv', 'values')

if __name__ == "__main__":
    main()  

