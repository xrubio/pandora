
#!/usr/bin/python

import fileinput, sys, os, random

## usage : $>python main3.py moduleName
## moduleName without py extension
## module <moduleName> should be in current working dir

## import experiment definitions
################################

if(len(sys.argv)<2):
	print "No experiment module selected.\n"
	print "usage : $>python main3.py moduleName\n"
	print "moduleName without py extension\n"
	print "module <moduleName> should be in current working dir\n"
	sys.exit(-1)
else:
	selectedModule = sys.argv[1]	

print "loading : " + selectedModule + "\n"

#scriptpath = "./variables.py"

scriptpath = "./"+selectedModule+".py"

myVars = __import__(selectedModule)

################################


xmlTemplate = 'templates/config_template_mn_walk_gpfs.xml'
#runTemplate = 'templates/bsub_template_mean.cmd'

indexKey 		= 'INDEX'
initialDirKey	= 'INITIALDIR'
numExecutionKey = 'NUMEXEC'
numAgentsKey	= 'NUMAGENTS'
numStepsKey		= 'NUMSTEPS'
minimumBiomassKey = 'BIOMASS_MINIMUM'
meanKey			= 'MEAN'
stdDevKey		= 'STDDEV'
climateKey		= 'CLIMATESEED'
H5DirKey		= 'RESULTSFILEDIR'
logsDirKey		= 'LOGSDIR'
tiffDirKey		= 'TIFFDIR'
controlerKey 	= 'CTYPE'
horizonKey 		= 'HORIZON'
widthKey 		= 'WIDTH'


################################

def replaceKey( fileName, key, value ):
    for line in fileinput.FileInput(fileName,inplace=1):
        if key in line:
            line = line.replace(key, value)
        sys.stdout.write(line)
    fileinput.close()
################################

currentDir=os.getcwd()    
targetDir = currentDir + '/results';   
os.system('cd ' + targetDir)

index = 0
print 'creating test workbench'

random.seed()

#stddev = 1885.158
#mean   = 4643.848
#minimumBiomass = 0.1

for numExecution in range(0,numExecutions):
	# each n.execution  has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))   
	#targetExp->expSets->meanValues
	#targetExp->expSets->stdDevValues   
	# meanValues[experimentID] = <min,max,step>
	for clim in range(len(meansVect)):
		mean=meansVect[clim]
		stddev=stddevsVect[clim]
		for t in targetBin:
			for a in numAgents:
				for steps in stepsV:
					for mb in minBioMassVect:
						for hh in horizons:
							for ww in widths:
								runID = '/h'+hh+'w'+ww+'_numAgents'+str(a)+'_years'+str(y)+'_climate'+mean+'_'+stddev+'_mb'+mb+'_bin'+t+'_exec'+str(numExecution)
								dirName = targetDir + runID
								print 'creating gujarat instance: '+runID
								#dirName = targetDir + '/numAgents'+str(a)+'/years'+str(y)+'/bin'+t+'/exec'+str(numExecution)
								dirResults = dirName + '/data' 
								dirLogs = dirName + '/logs'
								dirTiff = binDir[t] + '/resources'
								os.system('mkdir -p '+dirName)
								os.system('mkdir -p '+dirResults)
								os.system('mkdir -p '+dirLogs)
								configName = dirName + '/config.xml'           
								os.system('cp '+xmlTemplate+' '+configName)
								#---------
								replaceKey(configName, numAgentsKey, str(a))
								replaceKey(configName, numStepsKey, str(steps))
								#---------
								replaceKey(configName, minimumBiomassKey, str(mb))
								replaceKey(configName, meanKey, str(mean))
								replaceKey(configName, stdDevKey, str(stddev))
								replaceKey(configName, numExecutionKey, str(numExecution))
								replaceKey(configName, climateKey, randomValue)   
								replaceKey(configName, H5DirKey, dirResults)
								replaceKey(configName, logsDirKey, dirLogs)
								replaceKey(configName, tiffDirKey, dirTiff)
								replaceKey(configName, controlerKey, controler)
								replaceKey(configName, horizonKey, hh)
								replaceKey(configName, widthKey, ww)
								#runName = dirName+'/bsub_gujarat.cmd'
								#                os.system('cp '+runTemplate+' '+runName)
								#                replaceKey(runName, indexKey, str(index))
								#                replaceKey(runName, initialDirKey, dirName)
								index += 1
								#END WHILE

print 'workbench done, writing task file'

index = 0
taskFile = open(taskFileName, 'w')
#for minimumBiomass in minimumBiomassValues:   
#    for mean in meanValues :
#        for stddev in stdDevValues:
taskFile.write('#!/bin/bash\n')


for t in targetBin:
	for a in numAgents:
		for y in years:
			for clim in range(len(meansVect)):
				mean=meansVect[clim]
				stddev=stddevsVect[clim]
				for mb in minBioMassVect:
					for hh in horizons:
						for ww in widths:
							for numExecution in range(0,numExecutions):
								runID = '/h'+hh+'w'+ww+'_numAgents'+str(a)+'_years'+str(y)+'_climate'+mean+'_'+stddev+'_mb'+mb+'_bin'+t+'_exec'+str(numExecution)
								dirName = targetDir + runID
								print 'writing gujarat instance: '+runID		
								taskFile.write(MN_environment+'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:'+environments[t]+';')
								taskFile.write('cd '+dirName+';'+binDir[t]+'/'+binary[t]+'\n')
								index += 1
								#END LOOP					
taskFile.close()
os.system('chmod +x '+taskFileName)

