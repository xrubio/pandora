#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore'
taskFile = 'knowledgeTransmission.txt'

numExecutions = 100

xmlTemplate = 'templates/config_template_knowledgeTransmission.xml'

climateKey = 'CLIMATESEED'
numExecutionKey = 'NUMEXEC'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf knowledgeTransmission/')
os.system('mkdir knowledgeTransmission')

index = 0
print 'creating test workbench'

for numExecution in range(0,numExecutions):
	# each n.execution has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))
	for knowledge in knowledgeValues:
		print 'creating gujaratCellphones instance: ' + str(index) + ' execution: ' + str(numExecution)
		dirName = 'knowledge/gujaratCellphones_ex'+str(numExecution)
		os.system('mkdir '+dirName)
		configName = dirName + '/config.xml'			
		os.system('cp '+xmlTemplate+' '+configName)
		replaceKey(configName, climateKey, randomValue)
		replaceKey(configName, numExecutionKey, str(numExecution))
		index += 1

print 'workbench done, submitting tasks'
index = 0

taskFile = open(taskFile, 'w')
for knowledge in knowledgeValues:
	for numExecution in range(0,numExecutions):
		print 'writing gujaratCellphones instance: ' + str(index) + ' execution: ' + str(numExecution)
		dirName = 'resources/gujaratCellphones_ex'+str(numExecution)
		taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujaratCellphones\n')
		index += 1
taskFile.close()

