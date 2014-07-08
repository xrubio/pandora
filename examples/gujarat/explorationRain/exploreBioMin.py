#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujarat/explorationRain'
taskFile = 'biomin.txt'

numExecutions = 1

#minimumBiomassValues = ['0.3','0.4','0.5']
minimumBiomassValues = ['0.5','0.6','0.8','0.9']
#minimumBiomassValues = ['0.0','0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0']

meanValues = ['11521']

#meanValues = ['4955'] #, '5775', '11521']
stdDevValues = ['1011']
#stdDevValues = ['1934'] # ,'1365', '1011']

xmlTemplate = 'templates/config_template_mn_biomin.xml'
#runTemplate = 'templates/bsub_template_biomin.cmd'

indexKey = 'INDEX'
initialDirKey = 'INITIALDIR'
numExecutionKey = 'NUMEXEC'
minimumBiomassKey = 'BIOMASS_MINIMUM'
meanKey = 'MEAN'
stdDevKey = 'STDDEV'
climateKey = 'CLIMATESEED'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf biomin/')
os.system('mkdir biomin')

index = 0
print 'creating test workbench'

random.seed()

for numExecution in range(0,numExecutions):
	# each n.execution  has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))	
	for minimumBiomass in minimumBiomassValues:
		for mean in meanValues :
			for stddev in stdDevValues:
				print 'creating gujarat instance: ' + str(index) + ' for minimum biomass: ' + minimumBiomass + ' mean: ' + mean + ' stddev: ' + stddev + ' and execution: ' + str(numExecution)
				dirName = 'biomin/results_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)
				os.system('mkdir '+dirName)
				configName = dirName + '/config.xml'			
				os.system('cp '+xmlTemplate+' '+configName)
				replaceKey(configName, minimumBiomassKey, minimumBiomass)
				replaceKey(configName, meanKey, mean)
				replaceKey(configName, stdDevKey, stddev)
				replaceKey(configName, numExecutionKey, str(numExecution))
				replaceKey(configName, climateKey, randomValue)

				#runName = dirName+'/bsub_gujarat.cmd'
				#os.system('cp '+runTemplate+' '+runName)
				#replaceKey(runName, indexKey, str(index))
				#replaceKey(runName, initialDirKey, dirName)
				index += 1

print 'workbench done, submitting tasks'
index = 0

taskFile = open(taskFile, 'w')
for minimumBiomass in minimumBiomassValues:	
	for mean in meanValues :
		for stddev in stdDevValues:
			for numExecution in range(0,numExecutions):
				print 'writing gujarat instance: ' + str(index) + ' with min biomass: ' + minimumBiomass + ' mean: ' + mean + ' stddev: ' + stddev + ' and execution: ' + str(numExecution)
				dirName = 'biomin/results_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)
				taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujarat\n')
				index += 1
taskFile.close()

