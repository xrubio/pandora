#!/bin/bash

rm results
for i in `seq 1 1 100`;
do
	echo 'EXEC: '$i
	./battleSim config.xml >> results
	echo 'END EXEC: '$i
done

