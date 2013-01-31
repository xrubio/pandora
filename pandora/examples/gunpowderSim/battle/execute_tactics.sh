#!/bin/bash

rm results_tactics
touch results_tactics

rm -rf data_tactics
mkdir data_tactics

# 1-2, 1-3, 1-4

let blue=1

for red in `seq 2 4`;
do
	echo 'EXEC: '$blue'-'$red
	mkdir './data_tactics/data_'$blue'_'$red
	# coh
	for coh in `seq 25 25 75`;
	do
		# iterations
		for i in `seq 1 1 10`;
		do
			# acc = 1
			let acc=1
			./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
			
			let acc=5
			./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
	
			let acc=10
			./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
		done
	done
	echo 'END EXEC: '$blue'-'$red
done

let blue=2
for red in `seq 3 4`;
do
	echo 'EXEC: '$blue'-'$red
	mkdir './data_tactics/data_'$blue'_'$red
	# coh
	for coh in `seq 25 25 75`;
	do
		# iterations
		for i in `seq 1 1 10`;
		do
			# acc = 1
			let acc=1
			./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
			
			let acc=5
			./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
	
			let acc=10
			./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
		done
	done
	echo 'END EXEC: '$blue'-'$red
done

let blue=3
let red=4
echo 'EXEC: '$blue'-'$red
mkdir './data_tactics/data_'$blue'_'$red
# coh
for coh in `seq 25 25 75`;
do
	# iterations
	for i in `seq 1 1 10`;
	do
		# acc = 1
		let acc=1
		./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
		
		let acc=5
		./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics

		let acc=10
		./battleSim 'tactics/config_'$blue'_'$red'/config_'$acc'_'$coh'.xml' >> results_tactics
	done
done
echo 'END EXEC: '$blue'-'$red

