#!/bin/bash
# @ job_name         = gujarat_16 
# @ initialdir       = .
# @ output           = gujarat_16.log
# @ error            = gujarat_16.err
# @ total_tasks      = 16
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:30:00

date
srun  ./gujarat
date
