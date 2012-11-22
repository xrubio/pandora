#!/bin/bash

#rm results_parameters
#touch results_parameters
#rm -rf data_parameters
#mkdir data_parameters

for index in `seq 1 1 9`;
do
	for coh in `seq 10 10 100`;
	do
		echo 'EXEC: coh:'$coh
		for acc in `seq 1 2 25`;
		do
			mkdir './data_parameters/data_'$coh'_'$acc
				./battleSim 'parameters/config_'$coh'/config_'$acc'.xml' >> results_parameters
			rm -rf './data_parameters/data_'$coh'_'$acc
		done
	echo 'END EXEC: coh:'$coh
	done
done

