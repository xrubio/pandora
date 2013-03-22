#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore'
taskFile = 'callsPerDayDrySeason.txt'

numExecutions = 20
conversationsWetSeasonValues = ['1','2','3','4', '5', '6', '7', '8', '9', '10']
callsPerDayDrySeasonValues = ['1', '2', '3', '4', '5']

xmlTemplate = 'templates/config_template_callsPerDayDrySeason.xml'

climateKey = 'CLIMATESEED'
numExecutionKey = 'NUMEXEC'
conversationsWetSeasonKey = 'CONVERSATIONSWETSEASON'
callsPerDayDrySeasonKey = 'CALLSPERDAYDRYSEASON'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf callsPerDayDrySeason/')
os.system('mkdir callsPerDayDrySeason')

index = 0
print 'creating test workbench'

for numExecution in range(0,numExecutions):
	# each n.execution has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))
	for conversationsWetSeason in conversationsWetSeasonValues:
		for callsPerDayDrySeason in callsPerDayDrySeasonValues:
			print 'creating gujaratCellphones instance: ' + str(index) + ' for conversationsWetSeason: ' + conversationsWetSeason + 'and callsPerDayDrySeason: ' + callsPerDayDrySeason + ' and execution: ' + str(numExecution)
			dirName = 'callsPerDayDrySeason/gujaratCellphones_callsPerDayDrySeason'+callsPerDayDrySeason+'_conversationsWetSeason'+conversationsWetSeason+'_ex'+str(numExecution)
			os.system('mkdir '+dirName)
			configName = dirName + '/config.xml'			
			os.system('cp '+xmlTemplate+' '+configName)
			replaceKey(configName, climateKey, randomValue)
			replaceKey(configName, conversationsWetSeasonKey, conversationsWetSeason)
			replaceKey(configName, callsPerDayDrySeasonKey, callsPerDayDrySeason)
			replaceKey(configName, numExecutionKey, str(numExecution))
			index += 1

print 'workbench done, submitting tasks'
index = 0

taskFile = open(taskFile, 'w')
for conversationsWetSeason in conversationsWetSeasonValues:
	for callsPerDayDrySeason in callsPerDayDrySeasonValues: 
		for numExecution in range(0,numExecutions):
			print 'writing gujaratCellphones instance: ' + str(index) + ' with conversationsWetSeason: ' + conversationsWetSeason + ' and callsPerDayDrySeason' + callsPerDayDrySeason + ' and execution: ' + str(numExecution)
			dirName = 'resources/gujaratCellphones_callsPerDayDrySeason'+callsPerDayDrySeason+'_conversationsWetSeason'+conversationsWetSeason+'_ex'+str(numExecution)
			taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujaratCellphones\n')
			index += 1
taskFile.close()

