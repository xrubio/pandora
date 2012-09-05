import sys, os

def writeRunCmd( numExecution, homeRange, splitChance, cannibalism ):
	os.system("mkdir data_"+str(numExecution))
	newRunCmd = open("data_"+str(numExecution)+"/run_"+str(numExecution)+".cmd",'w')
	
	newRunCmd.write("#!/bin/bash\n")
	newRunCmd.write("# @ job_name         = ned_"+str(numExecution)+"\n")
	newRunCmd.write("# @ initialdir       = .\n")
	newRunCmd.write("# @ output           = data_"+str(numExecution)+"/ned_"+str(numExecution)+".log\n")
	newRunCmd.write("# @ error            = data_"+str(numExecution)+"/ned_"+str(numExecution)+".err\n")
	newRunCmd.write("# @ total_tasks      = 4\n")
	newRunCmd.write("# @ cpus_per_task    = 1\n")
	newRunCmd.write("# @ tracing          = 0\n")
	newRunCmd.write("# @ wall_clock_limit = 00:05:00\n")
	newRunCmd.write("\n")
	newRunCmd.write("date\n")

	newRunCmd.write("srun  ./run.sh ned 100 128 500 data_"+str(numExecution)+" "+str(homeRange)+" "+str(homeRange)+" "+str(splitChance)+" "+str(splitChance)+" "+str(cannibalism)+" "+str(cannibalism)+"\n")
	newRunCmd.write("date\n")

	newRunCmd.close()

	newConfigFile = open("data_"+str(numExecution)+"/variables.cfg",'w')
	newConfigFile.write("numExecution-"+str(numExecution)+"\n")
	newConfigFile.write("homeRange-"+str(homeRange)+"\n")
	newConfigFile.write("splitChance-"+str(splitChance)+"\n")
	newConfigFile.write("cannibalism-"+str(cannibalism)+"\n")
	newConfigFile.close()

minHomeRange = 1
maxHomeRange = 1
intervalHomeRange = 1

minSplitChance = 2
maxSplitChance = 2
intervalSplitChance = 1

minCannibalism = 6
maxCannibalism = 6
intervalCannibalism = 1

numExecution = 0

for homeRange in range(minHomeRange, maxHomeRange+1, intervalHomeRange):
	for splitChance in range(minSplitChance, maxSplitChance+1, intervalSplitChance):
		for cannibalism in range(minCannibalism, maxCannibalism+1, intervalCannibalism):
			writeRunCmd(numExecution, homeRange, splitChance, cannibalism);
			numExecution = numExecution+1

for i in range(0,numExecution):
	os.system("mnsubmit data_"+str(i)+"/run_"+str(i)+".cmd")


