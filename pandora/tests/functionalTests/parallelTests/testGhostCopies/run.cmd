#!/bin/bash
# @ job_name         = testGhostCopies
# @ initialdir       = .
# @ output           = testGhostCopies.log
# @ error            = testGhostCopies.err
# @ total_tasks      = 4
# @ cpus_per_task    = 4
# @ tracing          = 0
# @ wall_clock_limit = 00:02:00

date
srun  ./testGhostCopies
date
