#!/usr/bin/env python3

import os, sys

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

from pyPandora import Simulation, Agent, World, Point2DInt, SimulationRecord, GlobalAgentStats, AgentNum, AgentMean, AgentSum, AgentStdDev, GlobalRasterStats, RasterMean, RasterSum

def main():
    record = SimulationRecord(100, False)
    record.loadHDF5('../data/anazasi.h5', True, True)

    agentResults = GlobalAgentStats(';')
    agentResults.addAnalysis(AgentNum())
    agentResults.addAnalysis(AgentMean('estimates'))
    agentResults.applyTo(record,'households.csv', 'Household')
    
    rasterResults = GlobalRasterStats(';')
    rasterResults.addAnalysis(RasterMean())
    rasterResults.addAnalysis(RasterSum())
    rasterResults.applyTo(record, 'water.csv', 'yield x100')

if __name__ == "__main__":
    main()  

