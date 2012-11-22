#!/bin/bash
# @ job_name         = epidemy_1.log 
# @ initialdir       = .
# @ output           = epidemy_1.log
# @ error            = epidemy_1.err
# @ total_tasks      = 1
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:30:00

date
srun  ./epidemy
date
