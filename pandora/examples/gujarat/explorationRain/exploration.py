#!/usr/bin/python
import fileinput, sys, os, random

numExecutions = 5

#minimumBiomassValues = ['0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0']
minimumBiomassValues = ['0.1', '0.2']

xmlTemplate = 'templates/config_template_mn_home.xml'
runTemplate = 'templates/bsub_template.cmd'

indexKey = 'INDEX'
initialDirKey = 'INITIALDIR'
numExecutionKey = 'NUMEXEC'
minimumBiomassKey = 'BIOMASS_MINIMUM'
climateKey = 'CLIMATESEED'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf results_*')

index = 0
print 'creating test workbench'

random.seed()

for numExecution in range(0,numExecutions):
	# each n.execution  has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))	
	for minimumBiomass in minimumBiomassValues:
		print 'creating gujarat instance: ' + str(index) + ' for minimum biomass: ' + minimumBiomass + ' and execution: ' + str(numExecution)
		dirName = 'results_biomin'+minimumBiomass+'_ex'+str(numExecution)
		os.system('mkdir '+dirName)
		configName = dirName + '/config.xml'			
		os.system('cp '+xmlTemplate+' '+configName)
		replaceKey(configName, minimumBiomassKey, minimumBiomass)
		replaceKey(configName, numExecutionKey, str(numExecution))
		replaceKey(configName, climateKey, randomValue)

		runName = dirName+'/bsub_gujarat.cmd'
		os.system('cp '+runTemplate+' '+runName)
		replaceKey(runName, indexKey, str(index))
		replaceKey(runName, initialDirKey, dirName)
		index += 1

print 'workbench done, submitting tasks'
index = 0
for minimumBiomass in minimumBiomassValues:
	for numExecution in range(0,numExecutions):
		print 'submitting gujarat instance: ' + str(index) + ' with min biomass: ' + minimumBiomass + ' and execution: ' + str(numExecution)
		dirName = 'results_biomin'+minimumBiomass+'_ex'+str(numExecution)
		os.system('bsub < '+dirName+'/bsub_gujarat.cmd')
		index += 1

