#!/usr/bin/python

import os, sys

outputDir = 'compiledResultsStddev'
dirBase = '/home/bsc21/bsc21887/pandora/examples/gujarat/explorationRain/output/stddev/'
resultsBase = 'stddev/'

numExecutions = 3

minimumBiomassValues = ['0.1']
meanValues = ['4913']
stdDevValues = ['1','100','200','300','400','500','600','700','800','900','1000','1100','1200','1300','1400','1500','1600', '1700', '1800', '1900', '2000', '2100', '2200', '2300', '2400', '2500', '2600', '2700', '2800', '2900','3000']

logsDir = outputDir+'/logs'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(logsDir)

index = 0
for numExecution in range(0,numExecutions):
	for minimumBiomass in minimumBiomassValues:	
		for mean in meanValues :
			for stddev in stdDevValues:
				print 'analyzing results for instance with: ' + str(index) + ' with min biomass: ' + minimumBiomass + ' mean: ' + mean + ' stddev: ' + stddev + ' and execution: ' + str(numExecution)
				fileName = dirBase + 'data_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)+'/gujarat.h5'
				outputAgents = outputDir+'/agents_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)+'.csv'
				outputRasters = outputDir+'/rasters_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)+'.csv'
				outputLogDir = dirBase+'/logs_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)
				outputResultsDir = resultsBase+'/results_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)
				os.system('../analysis/analysis '+fileName+' ' + outputAgents + ' ' + outputRasters)
				os.system('mkdir '+logsDir+'/biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution))
				os.system('cp '+outputLogDir+'/* '+logsDir+'/biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution))
				os.system('cp '+outputResultsDir+'/* '+logsDir+'/biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution))

