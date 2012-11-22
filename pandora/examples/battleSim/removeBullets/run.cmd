#!/bin/bash
# @ job_name         = removeBullets 
# @ initialdir       = .
# @ output           = removeBullets.log
# @ error            = removeBullets.err
# @ total_tasks      = 4
# @ cpus_per_task    = 1
# @ tracing	     = 0
# @ wall_clock_limit = 00:05:00

date
srun ./removeBullets 10 data 
date
echo "DONE"

