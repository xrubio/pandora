#!/usr/bin/python3



import os, subprocess

for configFile in os.listdir("."):
	if not configFile.endswith(".xml"):
		continue
	print('executing with config: ',configFile)
	subprocess.call(['../gujarat_tv', configFile])

