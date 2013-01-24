#!/usr/bin/python

import os, sys

outputDir = 'finalResults'
dirBase = '/home/bsc21/bsc21887/pandora/examples/gujarat/explorationRain/output/mean/'
resultsBase = 'mean/'

numExecutions = 1

minimumBiomassValues = ['0.1']

meanValues = ['0', '200', '400', '600', '800', '1000', '1200', '1400', '1600', '1800', '2000', '2200', '2400', '2600', '2800', '3000', '3200', '3400', '3600', '3800', '4000', '4200', '4400','4600','4800','5000','5200','5400','5600','5800','6000','6200','6400','6600','6800','7000']
stdDevValues = ['1885']

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

