#!/bin/bash
# @ job_name         = gujarat_1
# @ initialdir       = .
# @ output           = gujarat_1.log
# @ error            = gujarat_1.err
# @ total_tasks      = 1
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 05:00:00

date
srun  ./gujarat
date
