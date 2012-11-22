#!/bin/bash
# @ job_name         = battleSim 
# @ initialdir       = .
# @ output           = battleSim.log
# @ error            = battleSim.err
# @ total_tasks      = 4
# @ cpus_per_task    = 1
# @ tracing	     = 0
# @ wall_clock_limit = 00:05:00

date
srun ./battleSim 100 512 500 500 data 
date
echo "DONE"

