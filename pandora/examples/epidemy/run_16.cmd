#!/bin/bash
# @ job_name         = epidemy_16 
# @ initialdir       = .
# @ output           = epidemy_16.log
# @ error            = epidemy_16.err
# @ total_tasks      = 16
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:30:00

date
srun  ./epidemy
date
