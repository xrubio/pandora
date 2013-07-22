#!/usr/bin/python

import sys
import random
import argparse
import math
import xml.etree.ElementTree

sys.path.append('../../pyPandora/')
sys.path.append('../../')

from pyPandora import Simulation, Agent, World, Point2DInt

def enum(**enums):
	""" converts a sequence of values to an C++ style enum """
	return type('Enum', (), enums)

landUses = enum(eNothing=0, eFarming=1, ePastures=2)

class OasisConfig():
	def __init__(self):
		self._size = 0
		self._numSteps = 0

		# farmer config
		self._initFarmers = 0
		self._farmersStrength = 0.0
		self._farmersGrowthRate = 0.0
		self._farmersImmigrationRate = 0.0

		# herder config
		self._initHerders = 0
		self._herdersRelativeStrength = 0.0
		self._herdersGrowthRate = 0.0
		self._herdersImmigrationRate = 0.0
		self._samplingNumber = 10

		self._asabiyaIncrease = 0.2
		self._asabiyaDecay = 0.1

		# max depth of the recursivity function
		self._maxDepth = 3
		# maximum distance at which a cell can be used as a pasture from the central point of the Herder's activity
		#self._maxPastureDistance = 10.0
	
	def deserialize(self, xmlFile):
		tree = xml.etree.ElementTree.parse(xmlFile)
		root = tree.getroot()

		self._size = int(root.find('size').get('value'))
		self._numSteps = int(root.find('numSteps').get('value'))

		agents = root.find('agents')
		farmers = agents.find('farmers')
		self._initFarmers = int(farmers.get('init'))
		self._farmersStrength = float(farmers.get('strength'))
		self._farmersGrowthRate = float(farmers.get('growthRate'))
		self._farmersImmigrationRate = float(farmers.get('immigrationRate'))

		herders = agents.find('herders')
		self._initHerders = int(herders.get('init'))
		self._herdersRelativeStrength = float(herders.get('relativeStrength'))
		self._herdersGrowthRate = float(herders.get('growthRate'))
		self._herdersImmigrationRate = float(herders.get('immigrationRate'))

class OasisAgent(Agent):
	""" Base class for the Musical Chairs model """
	_growthCoefficient = 0.1

	def __init__(self, id, numId):
		Agent.__init__( self, id)
		self._numId = numId

		# variables
		self._aggressiveness = 0.5
		self._asabiya = 0.5
		self._hadConflict = False

		self._popSize = 10.0
		# percentage of needed cells that were successfully consumed last time step
		self._consumedResources = 1.0

	def increasePopSize(self):
		print self.id + ' step: ' + str(self.getWorld().currentStep)
		print 'previous popsize: ' + str(self._popSize) + ' consumed resources: ' + str(self._consumedResources)
		self._popSize = self._popSize*self._consumedResources
		print 'left after starvation: ' + str(self._popSize)
		self._popSize += self._growthCoefficient*self._popSize
		print self.id + ' after growth: ' + str(self._popSize)
		if self._popSize==0:
			self.remove()

	def updateAsabiya(self):
		# if conflict, logistic growth 
		if self._hadConflict:
			self._asabiya += self.getWorld()._config._asabiyaIncrease*self._asabiya*(1-self-_asabiya)
		# if not, exponential decay
		else:
			self._asabiya -= self._asabiya*self.getWorld()._config._asabiyaDecay


	def registerAttributes(self):
		self.registerIntAttribute('popSize')
		self.registerIntAttribute('asabiya x100')
		self.registerIntAttribute('aggressiveness x100')
		self.registerIntAttribute('resources x100')

	def serialize(self):
		self.serializeIntAttribute('popSize', int(self._popSize))
		self.serializeIntAttribute('asabiya x100', int(100.0*self._asabiya))
		self.serializeIntAttribute('aggressiveness x100', int(100*self._aggressiveness))
		self.serializeIntAttribute('resources x100', int(100*self._consumedResources))
	
class Herder(OasisAgent):
	def __init__(self, id, numId):
		OasisAgent.__init__( self, id, numId)

		# pasture used last odd step
		self._lastPasture = Point2DInt(-1, -1)
		self._pastures = []

	def appendNewPastures(self, potentialPastures, locationToExpand, depth):

		if depth>self.getWorld()._config._maxDepth:
			return
		""" This recursive method expands a location in order to find all the pastures linked to locationToExpand """
		candidates = []
#		tabs = ""
#		for i in range(0, depth):
#			tabs += '\t'
#		print tabs+'expanding: ' + str(locationToExpand._x) + '/' + str(locationToExpand._y)
		for i in range(-1, 2):
			for j in range(-1, 2):
				newLocation = Point2DInt(locationToExpand._x+i, locationToExpand._y+j)
#print 'append new pastures, checking: '+str(newLocation._x) + '/' + str(newLocation._y) + ' is ok: ' + str(self.getWorld().checkPosition(newLocation))
				# valid cell not used for pastures yet (nothing or farms)
				if self.getWorld().checkPosition(newLocation) and self.getWorld().getValue("landUse", newLocation)!=landUses.ePastures:
					visited = False
					for location in potentialPastures:
						if location._x==newLocation._x and location._y==newLocation._y:
							visited = True

					# distance not used, depth is used to cut the recursivity
					#if not visited and newLocation.distance(centralLocation)<self.getWorld()._config._maxPastureDistance:
					if not visited :
						candidates.append(newLocation)

#		print 'oeoe, candidates: ' + str(len(candidates))
		random.shuffle(candidates)
		for candidate in candidates:
			potentialPastures.append(candidate)
			self.appendNewPastures(potentialPastures, candidate, depth+1)

	def getPotentialPastures(self, location):
		# returns a list of potential pastures not used around location
		potentialPastures = []
		potentialPastures.append(location)
		self.appendNewPastures(potentialPastures, location, 0)
#		print 'beginning '+str(len(potentialPastures)) +' pastures for location: ' + str(location._x) + '/' + str(location._y)
#		for pasture in potentialPastures:
#			print '\tpasture: '+ str(pasture._x) + '/' + str(pasture._y)
#		print 'finished'
		return potentialPastures

	def getFarmingPercentage(self, potentialPastures):
		# returns the percentage of potential pastures occupied by farmers
		numFarmers = 0
		for pasture in potentialPastures:
			if self.getWorld().getValue("landUse", pasture)==landUses.eFarming:
				numFarmers += 1
		return float(numFarmers)/float(len(potentialPastures))

	def calculateUtility(self, distToCentre, farmingPercentage):
		# TODO computes the utility based on distance, farming percentage and current asabiya*aggresiveness
		return random.random()

	def choosePasture(self):
		maxValue = 0.0
		nextPasture = Point2DInt(-1,-1)
		self._pastures = []
		bestPastures = []
#print 'beginning sampling for herder: ' + self.id

		# last used pasture
		if self._lastPasture._x != -1:
			potentialPastures = self.getPotentialPastures(self._lastPasture)
			bestPastures = potentialPastures
			if len(potentialPastures)>= self._popSize:
				farmingPercentage = self.getFarmingPercentage(potentialPastures)
				maxValue = self.calculateUtility(self._lastPasture.distance(self.position), farmingPercentage)
				nextPasture = self._lastPasture
				bestPastures = potentialPastures

		# map sampling
		i = 0
		while i < self.getWorld()._config._samplingNumber:
			newPoint = Point2DInt(random.randint(0, self.getWorld()._config._size-1), random.randint(0, self.getWorld()._config._size-1))
			if self.getWorld().getValue("refuge", newPoint)==1:
				continue
			potentialPastures = self.getPotentialPastures(newPoint)
			# we need at least popsize pastures
			if len(potentialPastures)>= self._popSize:
				i = i+1
				farmingPercentage = self.getFarmingPercentage(potentialPastures)
				value = self.calculateUtility(newPoint.distance(self.position), farmingPercentage)
				#print 'sample: ' + str(i) + ' is pos: ' + str(newPoint._x) + '/' + str(newPoint._y) + ' has around: ' + str(len(potentialPastures)) + ' potential cells with a farming percentage of: ' + str(farmingPercentage) + ' and a utility of: ' + str(value)
				if value > maxValue:
					maxValue = value
					nextPasture = newPoint
					bestPastures = potentialPastures
		#print 'ending sampling for herder: ' + self.id + ' new pasture position: ' + str(nextPasture._x) + '/' + str(nextPasture._y)
		
		# all eNothing cells are used until needed number; eFarming cells will be used if conflict is sucessful
		self.position = nextPasture
		for pasture in bestPastures:
			if self.getWorld().getValue("landUse", pasture)!=landUses.ePastures:
				self._pastures.append(pasture)
				# TODO can we reuse landUse raster map?
				if self.getWorld().getValue("landUse", pasture)==landUses.eFarming:
					self.getWorld().setValue("conflicts", pasture, 1)
				self.getWorld().setValue("landUse", pasture, landUses.ePastures)
				self.getWorld().setValue("herders", pasture, self._numId)
				if len(self._pastures)==int(math.ceil(self._popSize)):
					break
	
	def computeConsumedResources(self):
		# this method computes the percentage of needed cells that were successfully occupied last time step
		consumedCells = 0
		for pasture in self._pastures:
			if self.getWorld().getValue("landUse", pasture)==landUses.ePastures:
				consumedCells += 1
			if consumedCells==int(math.ceil(self._popSize)):
				break

		print 'consumed cells: ' + str(consumedCells) + ' for popsize: ' + str(self._popSize)
		self._consumedResources = min(1.0, float(consumedCells)/float(self._popSize))
		
	
	def moveToClosestRefuge(self):
		# this method updates position to the closest refuge to current location
		self._lastPasture = self.position

		closestDist = self.getWorld()._config._size*self.getWorld()._config._size
		newRefuge = Point2DInt(-1, -1)

		for i in range(0,self.getWorld()._config._size):
			for j in range(0,self.getWorld()._config._size):
				location = Point2DInt(i,j)
				if self.getWorld().getValue("refuge", location)==1:
					if location.distance(self.position)<closestDist:
						closestDist = location.distance(self.position)
						newRefuge = location

		self.position = newRefuge

	def updateState(self):
		if self.exists == False:
			return
		
		# even steps begin at refuge
		if self.getWorld().currentStep % 2 == 0:
			#print 'herder: ' + self.id + ' executing spring step'
			self.choosePasture()
			#print 'herder: ' + self.id + ' chose pasture: ' + str(self.position._x) + '/' + str(self.position._y)
		else:
			#print 'herder: ' + self.id + ' executing fall step'
			self.updateAsabiya()
			self.computeConsumedResources()
			self.increasePopSize()
			self.moveToClosestRefuge()
			#print 'herder: ' + self.id + ' chose refuge: ' + str(self.position._x) + '/' + str(self.position._y)
			
class Farmer(OasisAgent):
	def __init__(self, id, numId):
		OasisAgent.__init__( self, id, numId)

		self._lands = []
		
	
	def isBorder(self, cell):
		""" his method return true if the cell has neighboring cells with values != eFarming """
		for i in range(-1, 2):
			for j in range(-1, 2):
				newLocation = Point2DInt(cell._x+i, cell._y+j)

				# the cell is at the border of the world
				if not self.getWorld().checkPosition(newLocation):
					return True

				# if the neighbor is not in lands then it is a border
				alreadyInLands = False
				for land in self._lands:
					if newLocation._x==land._x and newLocation._y==land._y:
						alreadyInLands = True
				if not alreadyInLands:
					return True
		return False
		  
	def hasHerderNeighbor(self, cell):
		""" his method return true if the cell has neighboring cells with landUse values ePastures"""
		for i in range(-1, 2):
			for j in range(-1, 2):
				newLocation = Point2DInt(cell._x+i, cell._y+j)
				if self.getWorld().checkPosition(newLocation) and self.getWorld().getValue("landUse", newLocation)==landUses.ePastures:
					return True
		return False

	def computePotentialConflict(self):
		# this method returns the percentage of border adjacent to herders activities
		frontierPerimeter = 0
		potentialConflictCells = 0


		for land in self._lands:
			if self.isBorder(land):
				frontierPerimeter += 1
				if self.hasHerderNeighbor(land):
					potentialConflictCells += 1
		print self.id + ' - ' + str(potentialConflictCells) + ' from: ' + str(frontierPerimeter) + ' have herder neighbors from: ' + str(len(self._lands)) + ' farming lands'
		return float(potentialConflictCells)/float(frontierPerimeter)

	def expand(self, cell):
		candidates = []
		for i in range(-1, 2):
			for j in range(-1, 2):
				newLocation = Point2DInt(cell._x+i, cell._y+j)
				if self.getWorld().checkPosition(newLocation) and self.getWorld().getValue("landUse", newLocation)!=landUses.eFarming:
					candidates.append(newLocation)
		if len(candidates)==0:
		  return False
		random.shuffle(candidates)
		chosen = candidates[0]
		self._lands.append(chosen)
		self.getWorld().setValue("landUse", chosen, landUses.eFarming)
		self.getWorld().setValue("farmers", chosen, self._numId)
		return True

	def expandDecision(self, potentialConflict):
		# this evaluation is made using asabiya and aggresiveness
		# if a decision is made, controlled regions will be updated to popSize
		# if not, popSize will be decrased to the number of controlled regions

		neededLands = int(math.ceil(self._popSize)) - len(self._lands)
		print 'decision to expand with potential conflict: ' + str(potentialConflict) + ' needs to expand: ' + str(neededLands)

		while neededLands != 0:
			possibleExpansions = []
			for land in self._lands:
				possibleExpansions.append(land)
			while len(possibleExpansions)!=0:
				random.shuffle(possibleExpansions)
				land = possibleExpansions[0]
				if self.expand(land):
					neededLands -= 1
					break
				else:
					possibleExpansions.remove(land)
			# not enough land to expand
			if len(possibleExpansions)==0:
				print 'not enough land to expand with needed: ' + str(neededLands)
				return
	
	def computeConsumedResources(self):
		# this method computes the percentage of needed cells that were successfully occupied last time step
		consumedCells = 0
		for land in self._lands:
			if self.getWorld().getValue("landUse", land)==landUses.eFarming:
				consumedCells += 1
			if consumedCells==int(math.ceil(self._popSize)):
				break

		print 'consumed cells: ' + str(consumedCells) + ' for popsize: ' + str(self._popSize)
		self._consumedResources = min(1.0, float(consumedCells)/float(self._popSize))

	def updateState(self):	
		if self.exists == False:
			return
		
		if self.getWorld().currentStep % 2 == 0:
			potentialConflict = self.computePotentialConflict()
			self.expandDecision(potentialConflict)
		else:
			self.updateAsabiya()
			self.computeConsumedResources()
			self.increasePopSize()

class Oasis(World):

	def __init__(self, simulation, config ):
		World.__init__( self, simulation)
		self._config = config
		self._numFields = 0
		self._herdersOut = 0
		self._farmersOut = 0
		self._aggressions = 0
		self._invasions = 0

	def createRasters(self):
		self.registerDynamicRaster("refuge", 1)
		self.registerDynamicRaster("landUse", 1)
		self.registerDynamicRaster("conflicts", 1)
		# id of herder/farmer using the cell
		self.registerDynamicRaster("herders", 1)
		self.registerDynamicRaster("farmers", 1)

		self.getDynamicRaster("refuge").setInitValues(0, 1, 0)
		self.getDynamicRaster("landUse").setInitValues(0, landUses.ePastures, landUses.eNothing)
		self.getDynamicRaster("conflicts").setInitValues(0, 1, 0)
		self.getDynamicRaster("herders").setInitValues(0, self._config._initHerders, 0)		
		self.getDynamicRaster("farmers").setInitValues(0, self._config._initFarmers, 0)

		# we create 2 arbitrary refuges
		self.setValue("refuge", Point2DInt(5,5), 1)
		self.setValue("refuge", Point2DInt(self._config._size-5,self._config._size-5), 1)


		"""
		self.registerDynamicRaster("farmers", 1)
		self.registerDynamicRaster("herders", 1)
		self.registerDynamicRaster("conflicts", 1)
		self.getDynamicRaster("farmers").setInitValues(0, 1, 0)
		self.getDynamicRaster("herders").setInitValues(0, 1, 0)
		self.getDynamicRaster("conflicts").setInitValues(0, self._config._numSteps, 0)
		"""

	def createAgents(self):
		print 'creating farmers: '+str(self._config._initFarmers)+' and herders: '+str(self._config._initHerders)
		for i in range (1, 1+self._config._initFarmers):
			newAgent = Farmer('Farmer_'+str(i), i)
			self.addAgent(newAgent)
			newAgent.setRandomPosition()
			while self.getValue("refuge", newAgent.position)==1:
				newAgent.setRandomPosition()
			self.setValue('landUse', newAgent.position, landUses.eFarming)
			self.setValue('farmers', newAgent.position, i)
			newAgent._lands.append(newAgent.position)
			# expand to meet needed resources the first time step
			newAgent.expandDecision(0.0)

		for i in range (1, 1+self._config._initHerders):
			newAgent = Herder('Herder_'+str(i), i)
			self.addAgent(newAgent)
			if i % 2 == 0:
				newAgent.position = Point2DInt(5,5)
			else:
				newAgent.position = Point2DInt(self._config._size-5, self._config._size-5)
	
	def farmersMigration(self):
		return	
			
	def herdersMigration(self):
		return
	
	def resolveConflicts( self, position ):
		print 'resolving conflict at position: ' + str(position._x) + '/' + str(position._y)
		# just one of each per cell
		herder = self.getAgent(self.getAgentIds(position, 'Herder')[0])
		farmer = self.getAgent(self.getAgentIds(position, 'Farmer')[0])
		print 'conflict between herder: '+ herder.id + ' and farmer: ' + farmer.id + ' at pos: ' + str(position._x) + '/' + str(position._y)
		self.setValue('conflicts', position, self.getValue('conflicts', position)+1)

		indexOfOpportunity = self._numFields / (self._config._size*self._config._size)
		ratioOfStrengths = herder._strength/(herder._strength+farmer._strength)
		incentiveForMigration = 1 - ratioOfStrengths*indexOfOpportunity
		print '\tratio of strength: '+str(ratioOfStrengths) + ' incentive migration: ' + str(incentiveForMigration) + ' index of opportunity: ' + str(indexOfOpportunity) + ' num fields: ' + str(self._numFields) + ' size: ' + str(self._config._size)

		if incentiveForMigration>herder._aggressiveness:
			print '\therder migrated, removing agent: '+herder.id
			herder.remove()
			self.setValue('herders', position, 0)
			self._herdersOut += 1
			return

		# invasion attempt
		self._aggressions += 1
		randomValue = random.random()
		if herder._aggressiveness > randomValue:
			print '\therder invasion success with random: '+str(randomValue) + ' and aggressiveness: ' + str(herder._aggressiveness) + ' farmer removed: ' + farmer.id
			farmer.remove()
			self.setValue('farmers', position, 0)
			self._invasions += 1
			return
		else:
			print '\therder invasion failed with random: '+str(randomValue) + ' and aggressiveness: ' + str(herder._aggressiveness) + ' herder removed: ' + herder.id
			herder.remove()
			self.setValue('herders', position, 0)
			return
	
	def updateNumFields(self):
		self._numFields = 0
		for i in range(0,self._config._size):
			for j in range(0,self._config._size):
				position = Point2DInt(i,j)
				if self.getValue('farmers', position)==1 :
					self._numFields += 1
				
	def checkConflicts(self):
		self._aggressions = 0
		self._invasions = 0
		index = Point2DInt(0,0)
		for index._y in range(0,self._config._size):
			for index._x in range(0,self._config._size):
				if self.getValue('herders', index)==1 and self.getValue('farmers', index)==1:
					self.resolveConflicts(index)
	
	def shufflePositions(self):
		"""We need to shuffle the positions of the agents before checking conflicts"""
		listFarmers = []
		index = Point2DInt(0,0)
		for index._y in range(0,self._config._size):
			for index._x in range(0,self._config._size):
				if self.getValue('farmers', index)==1:
					aFarmer = self.getAgent(self.getAgentIds(index, 'Farmer')[0])
					self.setValue('farmers', index, 0)
					listFarmers.append(aFarmer)

		random.shuffle(listFarmers)
		index = Point2DInt(0,0)
		for i in range(0, len(listFarmers)):
			aFarmer = listFarmers[i]
			aFarmer.position = index
			self.setValue('farmers', index, 1)
			# next column
			if(index._x<(self._config._size-1)):
				index._x +=1
			# next row
			else:
			 	index._x = 0
			 	index._y += 1	
	
		listHerders = []
		index = Point2DInt(0,0)	
		for index._y in range(self._config._size-1, -1, -1):
			for index._x in range(self._config._size-1, -1, -1):
				if self.getValue('herders', index)==1:
					aHerder = self.getAgent(self.getAgentIds(index, 'Herder')[0])
					self.setValue('herders', index, 0)
					listHerders.append(aHerder)

		random.shuffle(listHerders)
		index = Point2DInt(self._config._size-1,self._config._size-1)
		for i in range(0, len(listHerders)):
			aHerder= listHerders[i]
			aHerder.position = index
			self.setValue('herders', index, 1)
			# next column
			if index._x > 0:
				index._x -=1
			# next row
			else:
			 	index._x = self._config._size-1
			 	index._y -= 1

	def conflict(self, herder, farmer, location):
		print 'conflict for location: ' + str(location._x) + '/' + str(location._y)
		# farmer wins
		if random.random()<-1:
			for pasture in herder._pastures:
				if pasture._x==location._x and pasture._y==location._y:
					herder._pastures.remove(pasture)
			self.setValue('landUse', location, landUses.eFarming)
			self.setValue('conflicts', location, 0)
			self.setValue('herders', location, 0)
			print 'farmer: ' + farmer.id + ' wins at herder: ' + herder.id + ' over landuse in location: ' + str(location._x) + '/' + str(location._y)
		else:
			for land in farmer._lands:
				if land._x==location._x and land._y==location._y:
					farmer._lands.remove(land)
			self.setValue('landUse', location, landUses.ePastures)
			self.setValue('conflicts', location, 0)
			self.setValue('farmers', location, 0)
			print 'herder: ' + herder.id + ' wins at farmer: ' + farmer.id + ' over landuse in location: ' + str(location._x) + '/' + str(location._y)

	def conflicts(self):
		index = Point2DInt(0,0)	
		for index._y in range(0,self._config._size):
			for index._x in range(0,self._config._size):
				if self.getValue('conflicts', index)==1:					
					herder = self.getAgent('Herder_'+str(self.getValue('herders', index)))
					farmer = self.getAgent('Farmer_'+str(self.getValue('farmers', index)))				
					print 'conflict between herder: '+ herder.id + ' and farmer: ' + farmer.id + ' at pos: ' + str(index._x) + '/' + str(index._y)
					self.conflict(herder, farmer, index)

	def resetPastureUse(self):
		"""this function resets to eNothing all cells used by pasture the previous year"""
		index = Point2DInt(0,0)	
		for index._y in range(0,self._config._size):
			for index._x in range(0,self._config._size):
				if self.getValue('landUse', index)==landUses.ePastures:
					self.setValue('landUse', index, landUses.eNothing)
					self.setValue('herders', index, 0)

	def stepEnvironment(self):
		print 'new step environment for step: ' + str(self.currentStep)
		# beginning of spring
		if self.currentStep % 2 == 0:
			self.resetPastureUse()
			return
		# beginning of fall, solve spring conflicts
		else:
			self.conflicts()

		#self.farmersMigration()
		#self.herdersMigration()

		#self.updateNumFields()

		#self.shufflePositions()
		#self.checkConflicts()

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('-x', '--config', default='config.xml', help='config file')
	args = parser.parse_args()
	config = OasisConfig()
	config.deserialize(args.config)

	print('size: '+str(config._size)+' num steps:'+str(config._numSteps))
	mySimulation = Simulation(config._size, config._numSteps)
	oasis = Oasis(mySimulation, config)
	oasis.initialize()
	oasis.run()
 
if __name__ == "__main__":
	main()

