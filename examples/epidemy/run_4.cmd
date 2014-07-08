#!/bin/bash
# @ job_name         = epidemy_4 
# @ initialdir       = .
# @ output           = epidemy_4.log
# @ error            = epidemy_4.err
# @ total_tasks      = 4
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:10:00

date
srun  ./epidemy
date
