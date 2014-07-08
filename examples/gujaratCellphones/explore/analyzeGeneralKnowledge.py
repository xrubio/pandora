#!/usr/bin/python

import os, sys

#numExecutions = 10
#knowledgeValues =  ['0','10','20','30','40','50','60','70','80','90','100']

numExecutions = 20
knowledgeValues =  ['0','10','20','30','40','50','60','70','80','90','100']

# dir where analysis will be stored
outputDir = 'compiledResultsGeneralKnowledge'
resultsDir = outputDir+'/generalKnowledge/'
logsDir = outputDir+'/logs/'
csvDir = outputDir+'/csv/'

# directory where raw results are located
rawDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore/results/generalKnowledge/'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(resultsDir)
os.makedirs(logsDir)
os.makedirs(csvDir)

index = 0
for numExecution in range(0,numExecutions):
	for knowledge in knowledgeValues:
		print 'analyzing results for instance: ' + str(index) + ' with general knowledge: ' + knowledge + ' and execution: ' + str(numExecution)
		suffix = 'knowledge'+knowledge+'_ex'+str(numExecution)
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

