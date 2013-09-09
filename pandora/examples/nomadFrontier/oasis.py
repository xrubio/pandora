#!/usr/bin/python

import sys
import random
import argparse
import math
import xml.etree.ElementTree

sys.path.append('/home/xrubio/workspace/pandora/pandora/pyPandora/')
sys.path.append('/home/xrubio/workspace/pandora/pandora/')

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
		self._numFarmers = 0
		self._farmersAggresiveness = 0.0
		self._farmersGrowthCoefficient = 0.0
		# herder config
		self._numHerders = 0
		self._herdersAggresiveness = 0.0
		self._herdersGrowthCoefficient = 0.0

		# conflict params
		self._asabiyaIncrease = 0.2
		self._asabiyaDecay = 0.1
		# relevance of distance to agent centre in case of conflict
		self._distanceSensibility = 2.0
		# threshold upon herder wins the conflict (if 0 equal power, if >0 farmers superiors to herders in conflict)
		self._conflictThreshold = 0.0

		# herder sampling param
		self._numSamples = 10
		# importance of distance related to conflict for herder decision making
		self._distParam = 0.0
		# max depth of the recursivity function
		self._maxDepth = 3
	
	def deserialize(self, xmlFile):
		tree = xml.etree.ElementTree.parse(xmlFile)
		root = tree.getroot()	
		
		self._resultsFile = str(root.find('output').get('resultsFile'))
		self._logsDir = str(root.find('output').get('logsDir'))

		self._size = int(root.find('size').get('value'))
		self._numSteps = int(root.find('numSteps').get('value'))
		self._serializeResolution = int(root.find('numSteps').get('serializeResolution'))

		conflict = root.find('conflict')
		self._asabiyaIncrease = float(conflict.get('asabiyaIncrease'))
		self._asabiyaDecay = float(conflict.get('asabiyaDecay'))
		self._distanceSensibility = float(conflict.get('distanceSensibility'))
		self._conflictThreshold = float(conflict.get('conflictThreshold'))

		herderSampling = root.find('herderSampling')
		self._numSamples = int(herderSampling.get('numSamples'))		
		self._distParam = float(herderSampling.get('distParam'))
		self._maxDepth = int(herderSampling.get('maxDepth'))
		
		agents = root.find('agents')
		farmers = agents.find('farmers')
		self._numFarmers = int(farmers.get('num'))
		self._farmersAggresiveness = float(farmers.get('aggressiveness'))
		self._farmersGrowthCoefficient = float(farmers.get('growthCoefficient'))

		herders = agents.find('herders')
		self._numHerders = int(herders.get('num'))
		self._herdersAggresiveness = float(herders.get('aggressiveness'))
		self._herdersGrowthCoefficient = float(herders.get('growthCoefficient'))

class OasisAgent(Agent):
	""" Base class for the Musical Chairs model """

	def __init__(self, id, numId):
		Agent.__init__( self, id)
		self._numId = numId

		# variables
		self._aggressiveness = 0.0
		self._growthCoefficient = 0.0
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
			self._asabiya += self.getWorld()._config._asabiyaIncrease*self._asabiya*(1-self._asabiya)
		# if not, exponential decay
		else:
			self._asabiya -= self._asabiya*self.getWorld()._config._asabiyaDecay
		self._hadConflict = False


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
		self._aggressiveness = 0.0

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
		# computes the utility based on distance, farming percentage and current asabiya*aggresiveness
		distWeight = 1-(distToCentre/self.getWorld()._maxDist)
		distWeight *= self.getWorld()._config._distParam
		# the weight of conflict is calculated as conflictAcceptance * (1-conflictAcceptance)*(1-conflictPossibility)
		conflictAcceptance = self._asabiya*self._aggressiveness
		conflictWeight = conflictAcceptance + (1-conflictAcceptance)*(1-farmingPercentage)
		conflictWeight *= (1.0-self.getWorld()._config._distParam)
		finalUtility = distWeight + conflictWeight
		print 'agent: ' + self.id + ' with asabiya: ' + str(self._asabiya) + ' aggresiveness: ' + str(self._aggressiveness) + ' farm percentage: ' + str(farmingPercentage) + ' and dist to centre: ' + str(distToCentre) + ' has dist weight: ' + str(distWeight) + ', conflict weight: ' + str(conflictWeight) + ' and final utility: ' + str(finalUtility)
		return finalUtility

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
		while i < self.getWorld()._config._numSamples:
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
		print 'ending sampling for herder: ' + self.id + ' new pasture position: ' + str(nextPasture._x) + '/' + str(nextPasture._y) + ' with utility: ' + str(maxValue)
		
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
			if self.exists == False:
				return
			self.moveToClosestRefuge()
			#print 'herder: ' + self.id + ' chose refuge: ' + str(self.position._x) + '/' + str(self.position._y)
			
class Farmer(OasisAgent):
	def __init__(self, id, numId):
		OasisAgent.__init__( self, id, numId)

		self._lands = []
		self._aggressiveness = 0.0
		
	
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
		print 'decision to expand with potential conflict: ' + str(potentialConflict) + ' needs to expand: ' + str(neededLands) + ' aggr: ' + str(self._aggressiveness) + ' asabiya: ' + str(self._asabiya)
		if potentialConflict > self._aggressiveness*self._asabiya:
			print 'potential conflict too high for agg: ' + str(self._aggressiveness) + ' and asabiya: ' + str(self._asabiya)
			return

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
		
		# decision of expanding is taken during fall
		if self.getWorld().currentStep % 2 == 0:
			return

		potentialConflict = self.computePotentialConflict()
		self.updateAsabiya()
		self.computeConsumedResources()
		self.increasePopSize()
		if self.exists == False:
				return
		self.expandDecision(potentialConflict)

class Oasis(World):

	def __init__(self, simulation, config ):
		World.__init__( self, simulation, 1, 1, config._resultsFile)
		self._config = config
		self._maxDist = math.sqrt(float(self._config._size*self._config._size))

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
		self.getDynamicRaster("herders").setInitValues(0, self._config._numHerders, 0)		
		self.getDynamicRaster("farmers").setInitValues(0, self._config._numFarmers, 0)

		# we create 2 arbitrary refuges
		self.setValue("refuge", Point2DInt(5,5), 1)
		self.setValue("refuge", Point2DInt(self._config._size-5,self._config._size-5), 1)

	def createAgents(self):
		print 'creating farmers: '+str(self._config._numFarmers)+' and herders: '+str(self._config._numHerders)
		for i in range (1, 1+self._config._numFarmers):
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
			newAgent._aggressiveness = random.uniform(0.0,self._config._farmersAggresiveness)
			newAgent._growthCoefficient = self._config._farmersGrowthCoefficient

		for i in range (1, 1+self._config._numHerders):
			newAgent = Herder('Herder_'+str(i), i)
			self.addAgent(newAgent)	
			newAgent._aggressiveness = random.uniform(0.0,self._config._herdersAggresiveness)
			newAgent._growthCoefficient = self._config._herdersGrowthCoefficient

			if i % 2 == 0:
				newAgent.position = Point2DInt(5,5)
			else:
				newAgent.position = Point2DInt(self._config._size-5, self._config._size-5)

	def computePower(self, agent, conflictLocation ):
		value = float(agent._popSize)*agent._asabiya
		value *= math.exp(-conflictLocation.distance(agent.position)/self._config._distanceSensibility)
		print 'agent: ' + agent.id + ' conflict in position: ' + str(conflictLocation._x) + '/' + str(conflictLocation._y) + ' at distance: ' + str(conflictLocation.distance(agent.position)) + ' with distance sens: ' + str(self._config._distanceSensibility) + ' asabiya: ' + str(agent._asabiya) + ' popsize: ' + str(agent._popSize)
		print 'agent: ' + agent.id + ' base value: ' + str(float(agent._popSize)*agent._asabiya) + ' exp: ' + str(-conflictLocation.distance(agent.position)/self._config._distanceSensibility) + ' exp: ' + str(math.exp(-conflictLocation.distance(agent.position)/self._config._distanceSensibility)) + ' final value: ' + str(value)
		return value


	def conflict(self, herder, farmer, location):
		print 'conflict for location: ' + str(location._x) + '/' + str(location._y)
		postHerder = self.computePower(herder, location)
		postFarmer = self.computePower(farmer, location)

		# farmer wins
		if postHerder-postFarmer<=self._config._conflictThreshold:
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
					if len(farmer._lands)==0:
						farmer.remove()
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
					herder._hadConflict = True
					farmer._hadConflict = True

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

