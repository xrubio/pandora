#!/usr/bin/python3

import sys, os


def lookForId( line ):
	idName = line.replace('<SimpleData name="id">','')	
	idName = idName.replace('</SimpleData>','')
	idName = idName.strip()
	return idName

def lookForNumStep( line ):
	numStep = line.replace('<SimpleData name="num step">','')
	numStep =  numStep.replace('</SimpleData>','')
	numStep = numStep.strip()
	return int(numStep)

def getNextTimeStep(numStep, listNumSteps):
	index = listNumSteps.index(numStep)
	# last value
	if index==(len(listNumSteps)-1):
			return numStep
	return 1+listNumSteps[index+1]

def main():
	if(len(sys.argv)!=4):
		print 'error invalid number of arguments, usage: ./createAnimation.py input.shp epsg output.kml'
		return
	
	fileNameShp = sys.argv[1]
	fileNameInput = 'static_'+fileNameShp.replace('shp','kml')
	fileNameIntermediate = 'latlong_'+fileNameShp
	epsg = sys.argv[2]
	fileNameOutput = sys.argv[3]
	os.system('ogr2ogr -s_srs EPSG:'+epsg+' -t_srs EPSG:4326 '+fileNameIntermediate+' '+fileNameShp)
	os.system('ogr2ogr -f KML '+fileNameInput+' '+fileNameIntermediate);


	# first parse, register steps
	fileInput = open(fileNameInput,'r')

	openAgent = 0
	idFound = 'no'
	numStep = -1

	agents = {}
	for line in fileInput:
		if(openAgent == 0):
			if line.find('<Placemark>')!=-1:
				openAgent = 1
		else:
			#  look for id
			if(idFound=='no'):
				if line.find('<SimpleData name="id">') != -1:
					idFound = lookForId(line)
			if(numStep==-1):
				if line.find('<SimpleData name="num step">') != -1:		
					numStep = lookForNumStep(line)

			if line.find('</Placemark>') != -1:
				if idFound=='no' or numStep==-1:
					print 'error looking for agent'
					return
				print 'agent processed with id:' + idFound + ' and num step: ' + str(numStep)
				if idFound in agents:
					listNumSteps = agents[idFound]
					listNumSteps.append(numStep)
					agents[idFound] = listNumSteps
				else:
					newListNumSteps = []
					newListNumSteps.append(numStep)
					agents[idFound] = newListNumSteps
					
				
				numStep = -1
				idFound = 'no'
				openAgent = 0

	fileInput.close()

	# sort num steps (in case agents are disordered)
	for key, value in agents.items():
		value.sort()

	# second parse, add timespans
	fileInput = open(fileNameInput,'r')
	fileOutput = open(fileNameOutput,'w')

	for line in fileInput:
		if(openAgent == 0):
			if line.find('<Placemark>')!=-1:
				openAgent = 1
		else:
			#  look for id
			if(idFound=='no'):
				if line.find('<SimpleData name="id">') != -1:
					idFound = lookForId(line)
			if(numStep==-1):
				if line.find('<SimpleData name="num step">') != -1:		
					numStep = lookForNumStep(line)

			if line.find('</Placemark>') != -1:
				if idFound=='no' or numStep==-1:
					print 'error looking for agent'
					return
				print 'agent processed with id:' + idFound + ' and num step: ' + str(numStep)
				fileOutput.write('\t<TimeSpan>\n')
				fileOutput.write('\t\t<begin>'+str(numStep)+'</begin>\n')
				fileOutput.write('\t\t<end>'+str(getNextTimeStep(numStep, agents[idFound]))+'</end>\n')
				fileOutput.write('\t</TimeSpan>\n')
				numStep = -1
				idFound = 'no'
				openAgent = 0

		fileOutput.write(line)
	fileInput.close()
	fileOutput.close()

if __name__ == "__main__":
    main()
