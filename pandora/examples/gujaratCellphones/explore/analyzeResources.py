#!/usr/bin/python

import os, sys

numExecutions = 10
resourcesValues = ['1','2','3','4', '5', '6', '7', '8', '9', '10']

# dir where analysis will be stored
outputDir = 'compiledResultsResources'
resultsDir = outputDir+'/resources/'
logsDir = outputDir+'/logs/'
csvDir = outputDir+'/csv/'

# directory where raw results are located
rawDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore/results/resources/'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(resultsDir)
os.makedirs(logsDir)
os.makedirs(csvDir)

index = 0
for resources in resourcesValues:	
	for numExecution in range(0,numExecutions):
		print 'analyzing results for instance: ' + str(index) + ' with resources per animal: ' + resources + ' and execution: ' + str(numExecution)
#suffix = 'resources'+resources+'_ex'+str(numExecution)
		suffix = 'resources'+resources+'_ex'+str(numExecution)
		rawResultsDir = rawDir+'/data_'+suffix
		
		# analysis
		fileToAnalyze = rawResultsDir+'/phones.h5'
		csv = csvDir+'/agents_'+suffix+'.csv'		
		os.system('../analysis/analysis '+fileToAnalyze+' '+csv)
		
		# copy results
		os.system('cp -ar '+rawResultsDir+' '+resultsDir)

		# copy logs
		rawLogDir = rawDir+'/logs_'+suffix
 		os.system('cp -ar '+rawLogDir+' '+logsDir)

