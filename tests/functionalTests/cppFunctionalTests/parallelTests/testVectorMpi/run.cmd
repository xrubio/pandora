#!/bin/bash
# @ job_name         = testVectorMpi
# @ initialdir       = .
# @ output           = testVectorMpi.log
# @ error            = testVectorMpi.err
# @ total_tasks      = 4
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:02:00

date
srun  ./testVectorMpi
date
