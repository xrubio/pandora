#nameProgram = 'gujarat'

agents = ['HunterGatherer']
world = 'GujaratWorld'
namespaceAgents = ['Gujarat']

srcFiles = Split('main.cxx GujaratWorld.cxx GujaratConfig.cxx Climate.cxx GujaratAgent.cxx HunterGatherer.cxx MDPAction.cxx MoveHomeAction.cxx Sector.cxx ForageAction.cxx SettlementAreas.cxx HunterGathererMDPConfig.cxx HunterGathererMDPState.cxx HunterGathererMDPModel.cxx DoNothingAction.cxx AgentController.cxx HunterGathererProgrammedController.cxx HunterGathererDecisionTreeController.cxx HunterGathererMDPController.cxx GujaratDemographics.cxx OriginalDemographics.cxx CaloricRequirementsTable.cxx GujaratState.cxx HGMind.cxx HGOmniMindMap.cxx HGPartialMindMap.cxx HGMindFactory.cxx HunterGathererRandomMDPPolicyController.cxx HunterGathererSimpleRandomController.cxx HunterGathererMarkovMoveController.cxx')


# APFiles = Split('APMoveHomeAction.cxx EstablishPlotAction.cxx SowAction.cxx HarvestAction.cxx MaintainPlotAction.cxx CultivatedField.cxx AgroPastoralist.cxx') 

import os, sys
from subprocess import call

sys.path.append('../../scripts/')
import generateMpi 

release = ARGUMENTS.get('release', 1)
mareNostrum = ARGUMENTS.get('mn', 0)
extremeDebug = ARGUMENTS.get('edebug', 0)
useMpi = ARGUMENTS.get('mpi', 1)

walkArg = ARGUMENTS.get('walk', 'tv')

walkFlagsTable={
	'c'   : 'CLASSICWALK'
	,'lrc' : 'LRCLASSICWALK'
	,'lrv' : 'LRVICINITYWALK'
	,'v'   : 'VICINITYWALK'
	,'tv'  : 'TRENDVICINITYWALK'
	}

walkProgramTable={
	'c'   : 'gujarat_c'
	,'lrc' : 'gujarat_lrc'
	,'lrv' : 'gujarat_lrv'
	,'v'   : 'gujarat_v'
	,'tv'  : 'gujarat_tv'
	}

treeHeurEstim = ARGUMENTS.get('treeHeur','')
treeHeurFlagsTable = {
	'walkEstim' : 'WALKESTIM'
	,'treeIdun' : 'TREEIDUN'
	}

print "treeHeur :"+treeHeurEstim
heurFlag = ''
if(treeHeurEstim!=''):
	heurFlag = '-D ' + treeHeurFlagsTable[treeHeurEstim]
	print "heurFlag :"+heurFlag





nameProgram = [walkProgramTable[walkArg]]

walkFlag = '-D ' + walkFlagsTable[walkArg]

print "WALK IS : " + walkFlag + "\n"

## Enable/Disable MDPState Space reduction
reduccArg = ARGUMENTS.get('rdc','1')
if int(reduccArg)==1:
	print "REDUCC ENABLED " + "\n"
	reduccFlag = '-D ' + 'REDUCC'
else:
	reduccFlag = ''
	print "REDUCC DISABLED " + "\n"


if int(useMpi) == 1:
	env = Environment(ENV=os.environ, CXX='mpicxx')
else:
	env = Environment(ENV=os.environ, CXX='g++')
	
generateMPICodeBuilder = Builder(action=generateMpi.execute)
env.Append( BUILDERS = {'GenerateMPICode' : generateMPICodeBuilder})



linkFlags = Split('-fopenmp')
libs = Split('tinyxml pthread grass_gis grass_datetime')
if int(release) == 0:
	env['CCFLAGS'] = Split(' -g -O0 -Wall -DTIXML_USE_STL -fopenmp -DPANDORADEBUG')
	if int(extremeDebug)==1:
		env['CCFLAGS'] += ['-DPANDORAEDEBUG']
	libs += ['pandorad']
else:
	env['CCFLAGS'] = Split('-O3 -DTIXML_USE_STL -fopenmp')
	libs += ['pandora']

if int(useMpi)==1:
	env['CCFLAGS'] += ['-DPANDORAMPI']

env['CCFLAGS'] += ['-std=c++0x']

env['CCFLAGS'] += [walkFlag]

env['CCFLAGS'] += [reduccFlag]

env['CCFLAGS'] += [heurFlag]

includeDirs = Split('./ ../../ ../../../libs/libmdp/')

if int(mareNostrum)==1:		
	includeDirs += ['/apps/HDF5/1.8.10-mpi/OPENMPI/include']
	includeDirs += ['/apps/BOOST/1_52_0/include/']
	includeDirs += ['/apps/TINYXML/2.6.2/include']
	includeDirs += ['/apps/GDAL/1.9.2/include']
	includeDirs += ['/apps/GRASS/6.4.3RC2/grass-6.4.3RC2/include/']
	libs += ['hdf5','gdal','boost_filesystem','boost_system']
	libDirs = Split('../../ /apps/HDF5/1.8.10-mpi/OPENMPI/lib /apps/GDAL/1.9.2/lib /apps/TINYXML/2.6.2/lib')
	libDirs += ['/apps/BOOST/1_52_0/lib/']
	libDirs += ['/apps/GRASS/6.4.3RC2/grass-6.4.3RC2/lib/']
else:
	includeDirs += ['/usr/lib/grass64/include/','/usr/include/gdal/']
	libDirs = ['../../', '/usr/lib/grass64/lib/']

	if int(useMpi)==1:
		includeDirs += ['/usr/local/hdf5/include']
		libDirs += ['/usr/local/hdf5/lib/']
	libs += ['mpl']

# add the list of mpi code that must be generated & compiled
mpiAgentsSrc = ['mpiCode/FactoryCode.cxx']
agentsSrc = ['main.cxx']
for agent in agents:	
	if agent != '':
		agentsSrc.append(agent+".cxx")
		mpiAgentsSrc.append("mpiCode/"+agent+"_mpi.cxx")
env['namespaces'] = namespaceAgents
env.GenerateMPICode( target=mpiAgentsSrc, source=agentsSrc)
env.Depends(world+'.hxx',mpiAgentsSrc)
env.Program(nameProgram, mpiAgentsSrc+srcFiles, CPPPATH=includeDirs, LIBS=libs, LIBPATH=libDirs, LINKFLAGS=linkFlags)

