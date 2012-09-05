#!/bin/bash
# @ job_name         = testInitialData2
# @ initialdir       = .
# @ output           = testInitialData2.log
# @ error            = testInitialData2.err
# @ total_tasks      = 4
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:02:00

date
srun  ./testInitialData2
date
