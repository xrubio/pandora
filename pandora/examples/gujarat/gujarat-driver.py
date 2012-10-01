#!/usr/bin/python
import sys
import os
import glob

def main() :
	
	configs_dir = 'batch/configs'
	sims = [ ( os.path.join(configs_dir,name), name ) for name in os.listdir( configs_dir ) ]

	for sim_dir, name in sims :
		config_file_path = os.path.join( sim_dir, 'config.xml' )
		results_path = os.path.join( 'batch/results', name )
		if not os.path.exists( results_path ) :
			os.makedirs( results_path )
		else :
			print >> sys.stdout, "Simulation results", name, "are going to be clobbered"
			print >> sys.stdout, "in order to execute the simulation, please store them away"
			continue
		sim_cmd = './gujarat %s > sim.log'%config_file_path
		print >> sys.stdout, "Executing simulation", name
		rv = os.system( sim_cmd )
		os.system( 'mv *.state.log %s'%results_path )
		os.system( 'mv world.log %s'%results_path )
		os.system( 'mv sim.log %s'%results_path )
		os.system( 'mv data/gujarat.h5 %s'%os.path.join( results_path, '%s.h5'%name ) )
		os.system( 'mv data/agents-0.abm %s'%results_path )
		os.system( 'rm -rf *.log' )

if __name__ == '__main__' :
	main()
