#!/usr/bin/python

import os, sys

outputDir = 'results_stddev'
dirBase = '/gpfs/projects/bsc21/WORK-RUBIO/'
resultsBase = '.'

numExecutions = 1
minimumBiomassValues = ['0.1']
meanValues = ['4643']
stdDevValues = ['1885']

logsDir = outputDir+'/logs'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(logsDir)

index = 0
for numExecution in range(0,numExecutions):
	for minimumBiomass in minimumBiomassValues:
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

