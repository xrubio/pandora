#!/usr/bin/python
import os
import sys
import glob
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D as Line2D 
from   GujaratStats import Agent, get_sim_length

def acquire_starvation_data_from_logs( logdir ) :

	data = []
	max_r = 0
	max_t = 0

	if not os.path.isdir( logdir ) :
		raise RuntimeError, "%s not a directory!"%logdir

	state_logs = glob.glob( os.path.join( logdir, "*.state.log" ) )
	agents = []
 	for log in state_logs : 
		print >> sys.stdout, "Processing", log
		agents.append( Agent( log ) )

	max_t = get_sim_length(agents)
	
	for i in range(0,max_t) :
		agent_count = 0
		starving_count = 0
		for a in agents :
			if i < len(a.time_steps ) :
				agent_count += 1
				starving_count += a.time_steps[i].isStarving
		if agent_count == 0 : agent_count = 1
		v = (float(starving_count)/float(agent_count))*100.0
		if v > max_r : max_r = v
		data.append(v)

	return  data,  max_t, max_r

def main() :
	
	if len(sys.argv) < 2 :
		print >> sys.stderr, "No input file was specified"
		print >> sys.stderr, "Usage: ./plot-avg-resources.py <world log> \"title \""
		sys.exit(1)

	inputs = []
	i = 1
	while i < len(sys.argv) :
		inputs.append( ( sys.argv[i], sys.argv[i+1] ) )
		print inputs[-1]
		i+=2

	datas = []
	global_xmax = 0
	global_ymax = 0

	for log, title_str in inputs :
		agents, xmax, ymax = acquire_starvation_data_from_logs( log )
		plt.plot( agents, label=title_str+", pct. agents starving" )
		if xmax > global_xmax : global_xmax = xmax
		if ymax > global_ymax : global_ymax = ymax
	
	plt.title( 'GujaratSim: Starvation' )
	plt.xlabel( 'TimeStep' )
	plt.ylabel( 'Percetange' )	
	plt.axis( [ 0, global_xmax, 0, global_ymax*1.10 ] )	
	plt.legend()

	plt.show()
	

if __name__ == '__main__' :
	main()
