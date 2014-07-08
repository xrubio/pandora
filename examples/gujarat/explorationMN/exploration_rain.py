#!/usr/bin/python

# this script explores the impact of rain variation using 100 HGs, lineal decay of resources form water, a map of 1600x1600 and DecisionTree algorithm
import fileinput, sys, os, random

#mapSizes = ['400','800','1600']
mapSizes = ['1600']

numExecutions = 1
#minimumBiomassValues = ['0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0']
minimumBiomassValues = ['1.0']
meanRainValues = ['4643.84']

xmlTemplate = 'templates/config_template_rain.xml'

indexKey = 'INDEX'
initialDirKey = 'INITIALDIR'
numExecutionKey = 'NUMEXEC'

climateKey = 'CLIMATESEED'
minimumBiomassKey = 'BIOMASS_MINIMUM'
meanRainKey = 'MEAN_RAIN'


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
		for meanRain in meanRainValues:
			print 'creating gujarat instance: ' + str(index) + ' mean rain: ' + meanRain + ' minimum biomass: ' + str(minimumBiomass) + ' and execution: ' + str(numExecution)
			dirName = 'results_meanRain_'+meanRain+'_biomin_'+minimumBiomass+'_ex'+str(numExecution)
			os.system('mkdir '+dirName)
			configName = dirName + '/config.xml'			
			os.system('cp '+xmlTemplate+' '+configName)
			replaceKey(configName, climateKey, randomValue)
			replaceKey(configName, minimumBiomassKey, str(minimumBiomass))
			replaceKey(configName, numExecutionKey, str(numExecution))
			replaceKey(configName, meanRainKey, str(meanRain))

			index += 1

print 'workbench done, submitting tasks'
index = 0
for minimumBiomass in minimumBiomassValues:
	for meanRain in meanRainValues:
		for numExecution in range(0,numExecutions):
			print 'submitting gujarat instance: ' + str(index) + ' and execution: ' + str(numExecution)
			dirName = 'results_meanRain_'+meanRain+'_biomin_'+minimumBiomass+'_ex'+str(numExecution)
			os.system('../gujarat '+dirName+'/config.xml >'+dirName+'/gujarat.log 2>'+dirName+'/gujarat.err')
			index += 1

