#!/usr/bin/python
import os
import sys
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D as Line2D 

def acquire_avg_resources_from_log( logfile ) :

	data = []
	timesteps = []
	max_r = 0
	max_t = 0

	with open( logfile) as instream :

		for line in instream :
			line = line.strip()
			key, value = line.split('=')
			if 'timestep' in key :
				timesteps.append( int(value) )
				if int(value) > max_t :
					max_t = int(value)
			if 'avgCurrent' in key :
				data.append( float(value) )
				if float(value) > max_r :
					max_r = float(value)

	return  data, max_t, max_r

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
		data, xmax, ymax = acquire_avg_resources_from_log( log )
		plt.plot( data, label=title_str )
		if xmax > global_xmax : global_xmax = xmax
		if ymax > global_ymax : global_ymax = ymax
	
	plt.title( 'GujaratSim: Yearly Cell Resources' )
	plt.xlabel( 'Timestep' )
	plt.ylabel( 'Avg. Resources' )	
	plt.axis( [ 0, global_xmax, 0, global_ymax*1.10 ] )	
	plt.legend()

	plt.show()
	

if __name__ == '__main__' :
	main()
