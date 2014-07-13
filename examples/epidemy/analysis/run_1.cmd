#!/bin/bash
# @ job_name         = analysisEpidemy.log 
# @ initialdir       = .
# @ output           = analysisEpidemy.log
# @ error            = analysisEpidemy.err
# @ total_tasks      = 1
# @ cpus_per_task    = 1
# @ tracing          = 0
# @ wall_clock_limit = 00:30:00

date
srun  ./statisticsEpidemy /gpfs/projects/bsc21/bsc21887/epidemy/data/epidemy.h5
date

