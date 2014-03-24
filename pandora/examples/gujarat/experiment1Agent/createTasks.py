#!/usr/bin/python3.3

path = 'parameterSweep'
numRuns = 10 
base = '/gpfs/projects/bsc21/WORK-RUBIO/'
for i in range(0, numRuns):
        print('ln -s /home/bsc21/bsc21887/pandora/examples/gujarat/gujarat_tv ',base,'/',path,'/run_',str(i).zfill(4), sep='')
for i in range(0, numRuns):
        print('cd ',base,'/',path,'/run_',str(i).zfill(4),' && ./gujarat_tv ', sep='')

