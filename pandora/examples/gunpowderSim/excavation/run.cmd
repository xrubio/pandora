#!/bin/bash
# @ job_name         = archaeoSim 
# @ initialdir       = .
# @ output           = archaeoSim.log
# @ error            = archaeoSim.err
# @ total_tasks      = 4
# @ cpus_per_task    = 1
# @ tracing	     = 0
# @ wall_clock_limit = 00:05:00

date
srun ./archaeoSim 15 true true ../degradedData/degradedBattlefield.h5 data 
date
echo "DONE"

