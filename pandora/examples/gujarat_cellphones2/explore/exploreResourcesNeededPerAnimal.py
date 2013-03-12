#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujarat_cellphones2/explore'
taskFile = 'resourcesNeededPerAnimal.txt'

numExecutions = 10

resourcesNeededPerAnimalValues = ['1','2','3','4', '5', '6', '7', '8', '9', '10']

xmlTemplate = 'templates/config_template_resourcesNeededPerAnimal.xml'

numExecutionKey = 'NUMEXEC'
resourcesNeededPerAnimalKey = 'RESOURCES_NEEDED_PER_ANIMAL'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf resourcesNeededPerAnimal/')
os.system('mkdir resourcesNeededPerAnimal')

index = 0
print 'creating test workbench'

for numExecution in range(0,numExecutions):
	for resourcesNeededPerAnimal in resourcesNeededPerAnimalValues:
		print 'creating gujarat_cellphones instance: ' + str(index) + ' for resources needed per animal: ' + resourcesNeededPerAnimal + ' and execution: ' + str(numExecution)
		dirName = 'resourcesNeededPerAnimal/results_resourcesNeededPerAnimal'+resourcesNeededPerAnimal+'_ex'+str(numExecution)
		os.system('mkdir '+dirName)
		configName = dirName + '/config.xml'			
		os.system('cp '+xmlTemplate+' '+configName)
		replaceKey(configName, resourcesNeededPerAnimalKey, resourcesNeededPerAnimal)
		replaceKey(configName, numExecutionKey, str(numExecution))
		index += 1

print 'workbench done, submitting tasks'
index = 0

taskFile = open(taskFile, 'w')
for resourcesNeededPerAnimal in resourcesNeededPerAnimalValues:
	for numExecution in range(0,numExecutions):
		print 'writing gujarat_cellphones instance: ' + str(index) + ' with resources needed per animal: ' + resourcesNeededPerAnimal + ' and execution: ' + str(numExecution)
		dirName = 'resourcesNeededPerAnimal/results_resourcesNeededPerAnimal'+resourcesNeededPerAnimal+'_ex'+str(numExecution)
		taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujarat_cellphones\n')
		index += 1
taskFile.close()

