#!/usr/bin/python3

import os

listTests = os.listdir('.')
print(listTests)

for test in listTests:
    if os.path.isdir(test) == True:
        os.chdir(test)
        print('cleaning environment for test: ',test)
        os.system('rm -rf logs mpiCode data logClean logCompilation logExecution')
        os.system('scons -c > logClean')
        print('compiling test: ',test)
        os.system('scons > logCompilation')
        print('executing test: ',test)
        os.system('mpirun -np 4 ./'+test + ' > logExecution')
        print('done: ',test)
        os.chdir('../')

