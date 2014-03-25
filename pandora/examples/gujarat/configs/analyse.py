#!/usr/bin/python3

import os, subprocess

for dataDir in os.listdir("."):
	if not dataDir.startswith("data"):
		continue
	print('analysing data from file: ',dataDir)
	params = dataDir[5:]
	dataFile = dataDir+'/gujarat.h5'
	outputFile = 'agents_'+params+'.csv'
	subprocess.call(['../analysis/analysis', dataFile,outputFile,'raster.csv'])

