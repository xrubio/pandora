#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore'
taskFile = 'resources.txt'

numExecutions = 10
resourcesValues = ['1','2','3','4', '5', '6', '7', '8', '9', '10']

xmlTemplate = 'templates/config_template_resources.xml'

climateKey = 'CLIMATESEED'
numExecutionKey = 'NUMEXEC'
resourcesKey = 'RESOURCES'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf resources/')
os.system('mkdir resources')

index = 0
print 'creating test workbench'

for numExecution in range(0,numExecutions):
	# each n.execution has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))
	for resources in resourcesValues:
		print 'creating gujarat_cellphones instance: ' + str(index) + ' for resources needed per animal: ' + resources + ' and execution: ' + str(numExecution)
		dirName = 'resources/gujaratCellphones_resources'+resources+'_ex'+str(numExecution)
		os.system('mkdir '+dirName)
		configName = dirName + '/config.xml'			
		os.system('cp '+xmlTemplate+' '+configName)
		replaceKey(configName, climateKey, randomValue)
		replaceKey(configName, resourcesKey, resources)
		replaceKey(configName, numExecutionKey, str(numExecution))
		index += 1

print 'workbench done, submitting tasks'
index = 0

taskFile = open(taskFile, 'w')
for resources in resourcesValues:
	for numExecution in range(0,numExecutions):
		print 'writing gujarat_cellphones instance: ' + str(index) + ' with resources needed per animal: ' + resources + ' and execution: ' + str(numExecution)
		dirName = 'resources/gujaratCellphones_resources'+resources+'_ex'+str(numExecution)
		taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujaratCellphones\n')
		index += 1
taskFile.close()

