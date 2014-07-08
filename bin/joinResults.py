#!/usr/bin/python
import os, sys, random
import argparse

# this script processes all the log simulations in one dir, and writes the values of one particular attribute into one single file.

def prepareProcess(inputDir,simulationFile, separator, output, attribute ):
	output = open(output, 'w')	
	simulation = open(inputDir+'/'+simulationFile, 'r')

	# headers
	output.write('var'+'\n')
	splittedHeader = simulation.readline().split(separator)
	for i in range(len(splittedHeader)):
		if splittedHeader[i] == attribute:
			indexAttribute = i
	
	for line in simulation:
		splittedLine = line.split(separator)
		output.write('step' + splittedLine[0]+'\n')
	output.close()
	return indexAttribute

def processSimulation(inputDir, simulationFile, separator, outputName, attributeIndex):
	output = open(outputName, 'r')
	outputTmp = open('tmp', 'w')
	simulation = open(inputDir+'/'+simulationFile, 'r')

	# header
	outputTmp.write(output.readline().strip('\n')+separator+simulationFile+'\n')
	simulation.readline()

	for simulationLine in simulation:
		previousLine = output.readline().strip('\n')+separator
		splittedLine = simulationLine.split(separator)
		value = splittedLine[attributeIndex]
		outputTmp.write(previousLine+value+'\n')
		
	simulation.close()	
	output.close()
	outputTmp.close()
	os.rename('tmp', outputName)

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('-i', '--input', default='input', help='directory where simulated files are stored')
	parser.add_argument('-o', '--output', default='results.csv', help='output file')
	parser.add_argument('-s', '--separator', default=';', help='separator token between values')
	parser.add_argument('-a', '--attribute', default='Number of agents', help='name of the attribute column to process')
	args = parser.parse_args()

	outputFile = open(args.output, 'w')
	outputFile.close()

	header = 0
	for root, dirs, simulationFiles in os.walk(args.input):
		for simulationFile in simulationFiles:
			if not simulationFile.endswith('.csv'):
				continue
			if header == 0:
				attributeIndex = prepareProcess(args.input,simulationFile, args.separator, args.output, args.attribute)
				header = 1
			print 'processing simulation results in file: ' + simulationFile
			processSimulation(args.input, simulationFile, args.separator, args.output, attributeIndex)

if __name__ == "__main__":
    main()

