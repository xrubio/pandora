#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore'
taskFile = 'phones.txt'

numExecutions = 40
knowledgeValues =  ['1','2','3']

xmlTemplate = 'templates/config_template_phones.xml'

climateKey = 'CLIMATESEED'
numExecutionKey = 'NUMEXEC'
knowledgeKey = 'KNOWLEDGE'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf phones')
os.system('mkdir phones')

index = 0
print 'creating test workbench'

for numExecution in range(0,numExecutions):
	# each n.execution has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))
	for knowledge in knowledgeValues:
		print 'creating gujaratCellphones instance: ' + str(index) + ' for knowledge: ' + knowledge + ' execution: ' + str(numExecution)
		dirName = 'phones/gujaratCellphones_out'+knowledge+'_ex'+str(numExecution)
		os.system('mkdir '+dirName)
		configName = dirName + '/config.xml'			
		os.system('cp '+xmlTemplate+' '+configName)
		replaceKey(configName, climateKey, randomValue)
		replaceKey(configName, knowledgeKey, knowledge)
		replaceKey(configName, numExecutionKey, str(numExecution))
		index += 1

print 'workbench done, submitting tasks'
index = 0

taskFile = open(taskFile, 'w')
for numExecution in range(0,numExecutions):
	for knowledge in knowledgeValues:
		print 'writing gujaratCellphones instance: ' + str(index) + ' for knowledge: ' + knowledge + ' execution: ' + str(numExecution)
		dirName = 'phones/gujaratCellphones_out'+knowledge+'_ex'+str(numExecution)
		taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujaratCellphones\n')
		index += 1
taskFile.close()

