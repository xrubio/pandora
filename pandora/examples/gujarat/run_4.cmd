#!/bin/bash
# @ job_name         = gujarat_4 
# @ initialdir       = .
# @ output           = gujarat_4.log
# @ error            = gujarat_4.err
# @ total_tasks      = 4
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 02:00:00

date
srun  ./gujarat
date
