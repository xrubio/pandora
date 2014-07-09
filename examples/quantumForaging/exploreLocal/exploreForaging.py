#!/usr/bin/python3
import fileinput, sys, os, random
baseDir = '.'

numExecutions = 10

riskValues = ['0.0', '0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0']
horizonValues = ['1','3','5','7','9','11']
widthValues = ['300']


xmlTemplate = 'templates/config_template.xml'

riskKey = 'RISK'
horizonKey = 'HORIZON'
widthKey = 'WIDTH'
numExecutionKey = 'NUMEXEC'

def replaceKey( fileName, key, value ):
	for line in fileinput.FileInput(fileName,inplace=1):
		if key in line:
			line = line.replace(key, value)
		sys.stdout.write(line)
	fileinput.close()

os.system('rm -rf riskExperiment/')
os.system('mkdir riskExperiment')

index = 0
print 'creating test workbench'

for numExecution in range(0,numExecutions):
	for risk in riskValues:
		for horizon in horizonValues:
			for width in widthValues:
				print 'creating quantum foraging experiment: ' + str(index) + ' width risk aversion: ' + risk + ' horizon: ' + horizon + ' width: ' + width + ' and execution: ' + str(numExecution)
				dirName = 'riskExperiment/forage_risk'+risk+'_horizon'+horizon+'_width'+width+'_ex'+str(numExecution)
				os.system('mkdir '+dirName)
				configName = dirName + '/config.xml'			
				os.system('cp '+xmlTemplate+' '+configName)
				replaceKey(configName, riskKey, risk)
				replaceKey(configName, horizonKey, horizon)
				replaceKey(configName, widthKey, width)
				replaceKey(configName, numExecutionKey, str(numExecution))
				index += 1

print 'workbench done, executing'
index = 0
for numExecution in range(0,numExecutions):
	for risk in riskValues:
		for horizon in horizonValues:
			for width in widthValues:
				print 'executing quantum foraging experiment: ' + str(index) + ' width risk aversion: ' + risk + ' horizon: ' + horizon + ' width: ' + width + ' and execution: ' + str(numExecution)
				dirName = 'riskExperiment/forage_risk'+risk+'_horizon'+horizon+'_width'+width+'_ex'+str(numExecution)
				os.system('cd ' + dirName + ' && ../../../quantumForager')
				index += 1

print 'execution done, analysing'

outputDir = 'compiledResults'
resultsDir = outputDir+'/data/'
logsDir = outputDir+'/logs/'
csvDir = outputDir+'/csv/'

os.system('rm -rf '+outputDir)
os.makedirs(outputDir)
os.makedirs(resultsDir)
os.makedirs(logsDir)
os.makedirs(csvDir)

index = 0
for numExecution in range(0,numExecutions):
	for risk in riskValues:
		for horizon in horizonValues:
			for width in widthValues:
				print 'analyzing quantum foraging experiment: ' + str(index) + ' width risk aversion: ' + risk + ' horizon: ' + horizon + ' width: ' + width + ' and execution: ' + str(numExecution)
				suffix = 'forage_risk'+risk+'_horizon'+horizon+'_width'+width+'_ex'+str(numExecution)
				rawResultsDir = 'riskExperiment/'+suffix+'/data/'
				rawLogsDir = 'riskExperiment/'+suffix+'/logs/'

				# analysis
				fileToAnalyze = rawResultsDir+'quantum.h5'
				csv = csvDir+'/agents_'+suffix+'.csv'		
				os.system('../analysisForager/analysis '+fileToAnalyze+' '+csv)
		
				# copy results
				os.system('mkdir '+resultsDir+'/'+suffix)
				os.system('cp -ar '+rawResultsDir+'/* '+resultsDir+'/'+suffix)

				os.system('mkdir '+logsDir+'/'+suffix)
				os.system('cp -ar '+rawLogsDir+'/* '+logsDir+'/'+suffix)


