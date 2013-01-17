#!/usr/bin/python

import os, sys

outputDir = 'finalResults'
dirBase = '/gpfs/projects/bsc21/WORK-RUBIO/datalogs_ex1/'
resultsBase = '.'

numExecutions = 1
minimumBiomassValues = ['0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0']
logsDir = outputDir+'/logs'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(logsDir)

index = 0
for numExecution in range(0,numExecutions):
	for minimumBiomass in minimumBiomassValues:
			print 'analyzing results for instance with: ' + str(index) + ' with min biomass: ' + minimumBiomass + ' and execution: ' + str(numExecution)
			fileName = dirBase + 'data_biomin'+minimumBiomass+'_ex'+str(numExecution)+'/gujarat.h5'
			outputAgents = outputDir+'/agents_biomin'+minimumBiomass+'_ex'+str(numExecution)
			outputRasters = outputDir+'/rasters_biomin'+minimumBiomass+'_ex'+str(numExecution)
			outputLogDir = dirBase+'/logs_biomin'+minimumBiomass+'_ex'+str(numExecution)
			os.system('../analysis/analysis '+fileName+' ' + outputAgents + ' ' + outputRasters)
			os.system('mkdir '+logsDir+'/biomin_'+minimumBiomass+'_ex'+str(numExecution))
			os.system('cp '+outputLogDir+'/* '+logsDir+'/biomin_'+minimumBiomass+'_ex'+str(numExecution))
#			os.system('cp '+resultsBase+'/* '+logsDir+'/biomin_'+minimumBiomass+'_ex'+str(numExecution))

