#!/usr/bin/python

import os, sys

outputDir = 'compiledResultsMean'
dirBase = '/home/bsc21/bsc21887/pandora/examples/gujarat/explorationRain/output/mean/'
resultsBase = 'mean/'

numExecutions = 1

minimumBiomassValues = ['0.1']

meanValues = ['1791', '2291', '2791', '3291', '3791', '4291', '4791', '5291', '5791','7791']
# local large
#meanValues = ['1791', '2291', '2791', '3291', '3791', '4291', '4791', '5291', '5791', '6291', '6791', '7291','7791']
# local small
#meanValues = ['4682','4707','4732','4757','4782','4807','4832','4857','4882','4907','4932','4957','4982','5002','5032']
# local stddev
stdDevValues = ['1934']

# regional large
#meanValues = ['2584','3084','3584','4084','4584','5084','5584','6084','6584','7084','7584','8084','8584']
# regional small
#meanValues = ['5457','5487','5517','5547','5577','5607','5637','5667','5697','5727','5757','5787','5817,5847']
# regional stddev
#stdDevValues = ['1365']

# continental large
#meanValues = ['8140','8640','9140','9640','10140','10640','11140','11640','12140','12640','13140','13640']
# continental small
#meanValues = ['10886','10946','11006','11066','11126','11186','11246','11306','11366','11426','11486','11546','11606','11666']
# continental stddev
#stdDevValues = ['1011']

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

