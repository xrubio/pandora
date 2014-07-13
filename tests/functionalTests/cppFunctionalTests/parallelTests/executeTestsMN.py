#!/usr/bin/python3

import os

listTests = os.listdir('.')
print listTests

for test in listTests:
	if os.path.isdir(test) == True:
		os.chdir(test)
		print 'cleaning environment for test: ' + test
		os.system('rm -rf '+test+'.err +'+test+'.log logs mpiCode data logClean logCompilation')
		os.system('scons -c > logClean')
		print 'compiling test: ' + test
		os.system('scons mn=1 release=0 > logCompilation')
		print 'sending test to MN cue: ' + test
		os.system('mnsubmit run.cmd')
		print 'done: ' + test
		os.chdir('../')

