#!/usr/bin/python

numExecutions = 4
mapSizes = ['1200']
numHGs = ['100']
controllers = ['DecisionTree']

os.system('rm -rf results')
os.system('mkdir results')

for numExecution in range(0,numExecutions):
	for mapSize in mapSizes:
		for numHG in numHGs:
			for controller in controllers:
				print 'analyzing results for instance with: ' + str(index) + ' for map: ' + mapSize + ' numHG: ' + numHG + ' with controller: ' + controller + ' and execution: ' + str(numExecution)
				fileName = 'data_results_size'+mapSize+'_numHG'+numHG+'_controller_'+controller+'_ex'+str(numExecution)+'/gujarat.h5'
				os.system('../analysisNew/statisticsGujarat '+fileName)
				os.system('mv agents.csv agents_size'+mapSize+'_numHG'+numHG+'_controller_'+controller+'_ex'+str(numExecution))
				os.system('mv resources.csv resources_size'+mapSize+'_numHG'+numHG+'_controller_'+controller+'_ex'+str(numExecution))

