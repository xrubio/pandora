#!/usr/bin/python
import sys
import os
import glob
from   GujaratStats import Agent, PopulationStats

def main() :
	
	if len(sys.argv) < 2 : 
		print >> sys.stderr, "Insufficient arguments supplied!"
		print >> sys.stderr, "Usage: ./computeStateStatistics.py <path>"
		sys.exit(1)

	input_path = sys.argv[1]
	print >> sys.stdout, "Input path:", input_path
	state_logs = glob.glob( os.path.join( input_path, "*.state.log" ) )
	print >> sys.stdout, len(state_logs), "agent state logs found..."

	agents = []

	for log in state_logs : 
		print >> sys.stdout, "Processing", log
		agents.append( Agent( log ) )

	for agent in agents :
		agent.computeStats(0,360)

	globalStats = PopulationStats( agents )
	globalStats.report( sys.stdout )

if __name__ == '__main__' :
	main()
