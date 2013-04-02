#!/usr/bin/python

import os, sys

numExecutions = 40
knowledgeValues =  ['1','2','3']

# dir where analysis will be stored
outputDir = 'compiledResultsPhones'
resultsDir = outputDir+'/phones/'
logsDir = outputDir+'/logs/'
csvDir = outputDir+'/csv/'

# directory where raw results are located
rawDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore/results/phones/'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(resultsDir)
os.makedirs(logsDir)
os.makedirs(csvDir)

index = 0
for numExecution in range(0,numExecutions):
	for knowledge in knowledgeValues:
		print 'analyzing results for instance: ' + str(index) + ' with general knowledge: ' + knowledge + ' and execution: ' + str(numExecution)
		suffix = 'out'+knowledge+'_ex'+str(numExecution)
		rawResultsDir = rawDir+'/data_'+suffix
		
		# analysis
		fileToAnalyze = rawResultsDir+'/phones.h5'
		csv = csvDir+'/agents_'+suffix+'.csv'
		os.system('../analysisHerder/analysis '+fileToAnalyze+' '+csv)
		
		# copy results
		os.system('cp -ar '+rawResultsDir+' '+resultsDir)

		# copy logs
		rawLogDir = rawDir+'/logs_'+suffix
		os.system('cp -ar '+rawLogDir+' '+logsDir)

