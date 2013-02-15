#!/usr/bin/python
import fileinput, sys, os, random
baseDir = '/home/bsc21/bsc21887/pandora/examples/gujarat/explorationRain'
taskFile = 'mean.txt'

numExecutions = 5

minimumBiomassValues = ['0.1']

# local large
#meanValues = ['1791', '2291', '2791', '3291', '3791', '4291', '4791', '5291', '5791','7791']
# local small
meanValues = ['4682','4707','4732','4757','4782','4807','4832','4857','4882','4907','4932','4957','4982','5002','5032']
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

xmlTemplate = 'templates/config_template_mn_mean.xml'
#runTemplate = 'templates/bsub_template_mean.cmd'

indexKey = 'INDEX'
initialDirKey = 'INITIALDIR'
numExecutionKey = 'NUMEXEC'
minimumBiomassKey = 'BIOMASS_MINIMUM'
meanKey = 'MEAN'
stdDevKey = 'STDDEV'
climateKey = 'CLIMATESEED'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf mean/')
os.system('mkdir mean')

index = 0
print 'creating test workbench'

random.seed()

for numExecution in range(0,numExecutions):
	# each n.execution  has the same seed in all parameter space
	randomValue = str(random.randint(0,10000000))	
	for minimumBiomass in minimumBiomassValues:
		for mean in meanValues :
			for stddev in stdDevValues:
				print 'creating gujarat instance: ' + str(index) + ' for minimum biomass: ' + minimumBiomass + ' mean: ' + mean + ' stddev: ' + stddev + ' and execution: ' + str(numExecution)
				dirName = 'mean/results_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)
				os.system('mkdir '+dirName)
				configName = dirName + '/config.xml'			
				os.system('cp '+xmlTemplate+' '+configName)
				replaceKey(configName, minimumBiomassKey, minimumBiomass)
				replaceKey(configName, meanKey, mean)
				replaceKey(configName, stdDevKey, stddev)
				replaceKey(configName, numExecutionKey, str(numExecution))
				replaceKey(configName, climateKey, randomValue)

#runName = dirName+'/bsub_gujarat.cmd'
#				os.system('cp '+runTemplate+' '+runName)
#				replaceKey(runName, indexKey, str(index))
#				replaceKey(runName, initialDirKey, dirName)
				index += 1

print 'workbench done, writing task file'

index = 0
taskFile = open(taskFile, 'w')
for minimumBiomass in minimumBiomassValues:	
	for mean in meanValues :
		for stddev in stdDevValues:
			for numExecution in range(0,numExecutions):
				print 'writing gujarat instance: ' + str(index) + ' with min biomass: ' + minimumBiomass + ' mean: ' + mean + ' stddev: ' + stddev + ' and execution: ' + str(numExecution)
				dirName = 'mean/results_biomin'+minimumBiomass+'_mean'+mean+'_stddev'+stddev+'_ex'+str(numExecution)
				taskFile.write('cd '+baseDir+'/'+dirName+' && ../../../gujarat\n')
				index += 1
taskFile.close()
