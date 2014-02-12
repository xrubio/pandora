
taskFileName = 'run.bash'
binDir = {					
	'FULL':'/home/atorrano/research/projects/simulpast/gujarat/workspace/pandora/devel/ForageWalk/pandora/pandora/examples/gujarat'
	,'RDC':'/home/atorrano/research/projects/simulpast/gujarat/workspace/pandora/devel/ForageWalk/pandora/pandora/examples/gujarat'
	}   

MN_environment ='' 
#'module load BOOST;module load HDF5/1.8.10-mpi;module load PYTHON/3.3.2;module load openmpi/1.7.2;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/apps/BOOST/1.52.0/lib:/apps/HDF5/1.8.10-mpi/OPENMPI/lib:/apps/TINYXML/2.6.2/lib:/apps/GRASS/6.4.3RC2/grass-6.4.3RC2/lib:/apps/OPENMPI/1.7.2/lib:/apps/GDAL/1.9.2/lib:/apps/PYTHON/3.3.2/lib;export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/apps/BOOST/1.52.0/include:/apps/HDF5/1.8.10-mpi/OPENMPI/include:/apps/TINYXML/2.6.2/include:/apps/GRASS/6.4.3RC2/grass-6.4.3RC2/include:/apps/OPENMPI/1.7.2/include:/apps/GDAL/1.9.2/include:/apps/PYTHON/3.3.2/include/python3.3m;'

binary = {
	'FULL':'gujarat_full'
	,'RDC':'gujarat_rdc'		
	}    
    
CSVbinDir = binDir[0] + '/analysis'

numExecutions 	= 2 
numAgents 	= [1]
stepsVect	= [1]
targetBin 	= ['FULL','RDC']
meansVect 	= ['4643.848']
stddevsVect 	= ['1.5']
minBioMassVect 	= ['0.1']

horizons = ['1','3','5']
widths = ['2','7'] ## width=1 produces an assert violation

xmlTemplate 	= 'templates/config_template_rdc.xml'
#runTemplate = 'templates/bsub_template_mean.cmd'
controler 	= 'MDP'
environments 	= {
		'FULL' :'/home/atorrano/research/projects/simulpast/gujarat/workspace/pandora/devel/ForageWalk/pandora/pandora'
		,'RDC' :'/home/atorrano/research/projects/simulpast/gujarat/workspace/pandora/devel/ForageWalk/pandora/pandora'
		}




