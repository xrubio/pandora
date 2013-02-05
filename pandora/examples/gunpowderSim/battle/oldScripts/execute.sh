#!/bin/bash

rm -rf data*
rm -rf results
touch results

for accuracy in `seq 10 10 100`;
do
	for cohesion in `seq 10 10 100`;
	do
		mkdir 'data_'$cohesion'_'$accuracy
	done
done
for accuracy in `seq 10 10 100`;
do
	for cohesion in `seq 10 10 100`;
	do
		./battleSim 'config_'$cohesion'/config_'$cohesion'_'$accuracy'.xml' >> results &
	done
done

