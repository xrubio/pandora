#!/usr/bin/python

import os, sys

numExecutions = 10
rainStddevValues = ['1','100', '200', '300','400', '500', '600', '700', '800', '900', '1000', '1100', '1200', '1300', '1400', '1500', '1600', '1700', '1800', '1900', '2000', '2100', '2200', '2300', '2400', '2500']

# dir where analysis will be stored
outputDir = 'compiledResultsRainStddev'
resultsDir = outputDir+'/rainStddev/'
logsDir = outputDir+'/logs/'
csvDir = outputDir+'/csv/'

# directory where raw results are located
rawDir = '/home/bsc21/bsc21887/pandora/examples/gujaratCellphones/explore/results/rainStddev/'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(resultsDir)
os.makedirs(logsDir)
os.makedirs(csvDir)

index = 0
for numExecution in range(0,numExecutions):
	for rainStddev in rainStddevValues:
		print 'analyzing results for instance: ' + str(index) + ' with rain std dev: ' + rainStddev + ' and execution: ' + str(numExecution)
		suffix = 'rainStddev'+rainStddev+'_ex'+str(numExecution)
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

