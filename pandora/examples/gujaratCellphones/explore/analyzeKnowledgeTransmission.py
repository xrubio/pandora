#!/usr/bin/python

import os, sys

#numExecutions = 100
numExecutions = 4

# dir where analysis will be stored
outputDir = 'compiledResultsKnowledge'
resultsDir = outputDir+'/knowledge/'
logsDir = outputDir+'/logs/'
csvDir = outputDir+'/csv/'

# directory where raw results are located
rawDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore/results/knowledge/'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(resultsDir)
os.makedirs(logsDir)
os.makedirs(csvDir)

index = 0
#for numExecution in range(0,numExecutions):
for numExecution in range(0,numExecutions):
	print 'analyzing results for instance: ' + str(index) + ' and execution: ' + str(numExecution)
	suffix = 'ex'+str(numExecution)
	rawResultsDir = rawDir+'/data_'+suffix
		
	# analysis
	fileToAnalyze = rawResultsDir+'/phones.h5'
	csv = csvDir+'/villages_'+suffix+'.csv'		
	os.system('../analysisVillage/analysis '+fileToAnalyze+' '+csv)
		
	# copy results
	os.system('cp -ar '+rawResultsDir+' '+resultsDir)

	# copy logs
	rawLogDir = rawDir+'/logs_'+suffix
	os.system('cp -ar '+rawLogDir+' '+logsDir)

