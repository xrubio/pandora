#!/usr/bin/python

import sys
import random
import argparse
import xml.etree.ElementTree
import math

sys.path.append('../../pyPandora/')
sys.path.append('../../')

from pyPandora import Simulation, Agent, World, Point2DInt

def enum(**enums):
	""" converts a sequence of values to an C++ style enum """
	return type('Enum', (), enums)

terrainTypes = enum(ePastures=0, eFields=1)

class FrontierConfig():
	_size = 22
	_numSteps = 10

	# r_0 in the logistic growth model
	_asabiyaIncrease = 0.2
	# exponential decay of asabiyta
	_asabiyaDecay = 0.1
	# parameter fixing the influence of distance from political center to asabiya
	_distanceSensibility = 2
	# the attack value must be superior to attackThreshold in order to conquer a neighbouring cell
	_attackThreshold = 0.1
	# minimal value of average asabiya of an empire
	_minAsabiya = 0.003

	def __init__(self):
		return
	
	def deserialize(self, xmlFile):
		"""
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
		"""
		return

class Empire(Agent):
	""" An empire created during the simulation """
	
	def __init__(self, id):
		Agent.__init__( self, id)

		self._averageAsabiya= 0.0
		self._empireId = 0
		self._numRegions = 1

	def updateNumRegions(self):
		self._numRegions = 0
		index = Point2DInt(0,0)
		for index._x in range(0, self.getWorld()._config._size):
			for index._y in range(0, self.getWorld()._config._size):
				if self.getWorld().getValue("id", index) == self._empireId:
					self._numRegions += 1
		print 'empire: ' + self.id + ' has: ' + str(self._numRegions) + ' regions'

	def calculateAverageAsabiya(self):
		self._averageAsabiya = 0.0

		index = Point2DInt(0,0)
		for index._x in range(0, self.getWorld()._config._size):
			for index._y in range(0, self.getWorld()._config._size):
				if self.getWorld().getValue("id", index) == self._empireId:
					self._averageAsabiya = self._averageAsabiya + float(self.getWorld().getValue("asabiya", index)/1000.0)
					print '\tadding: ' + str(float(self.getWorld().getValue("asabiya", index)/1000.0)) + " to average: " +  str(self._averageAsabiya)

		self._averageAsabiya = self._averageAsabiya/self._numRegions
		print "empire: " + self.id + " has average asabiya: " + str(self._averageAsabiya) + " containing: " + str(self._numRegions) + " regions"


	def calculateCentre(self):	
		centre = Point2DInt(0,0)

		index = Point2DInt(0,0)
		for index._x in range(0, self.getWorld()._config._size):
			for index._y in range(0, self.getWorld()._config._size):
				if self.getWorld().getValue("id", index) == self._empireId:
					centre._x = centre._x + index._x
					centre._y = centre._y + index._y

		centre._x = centre._x / self._numRegions 
		centre._y = centre._y / self._numRegions 
		self.position = centre
		print "centre of empire: " + self.id + " is: " +str(centre._x) + '/' + str(centre._y)

	def updateState(self):
		self.updateNumRegions()

		# if it has collapsed do nothing
		if self._numRegions==0 :
			return

		self.calculateAverageAsabiya()
		self.calculateCentre()

	def serialize(self):
		return

class Frontier(World):

	_config	= FrontierConfig()

	def __init__(self, simulation, config ):
		World.__init__( self, simulation)
		self._config = config
		self._numEmpires = 1

	def createRasters(self):
		self.registerDynamicRaster("id", 1)
		self.registerDynamicRaster("asabiya", 1)
		self.registerDynamicRaster("executed", 0)
		self.getDynamicRaster("id").setInitValues(0, 10000, 0)
		self.getDynamicRaster("asabiya").setInitValues(0, 10000, 500)
		self.getDynamicRaster("executed").setInitValues(0, 1, 0)

	def createAgents(self):
		newEmpire = Empire('Empire_1')
		newEmpire._empireId = self._numEmpires
		self.addAgent(newEmpire)
		#newEmpire.setRandomPosition()
		newEmpire.position = Point2DInt(1,1)
		self._numEmpires += 1

		# first empire has a 3x3 zone
		index = Point2DInt(0,0)
		print 'centre of empire : ' + str(newEmpire.position._x) + '/' + str(newEmpire.position._y)
		for index._x in range(newEmpire.position._x-1, newEmpire.position._x+2):
			for index._y in range(newEmpire.position._y-1, newEmpire.position._y+2):
				if self.checkPosition(index):
					self.setValue("id", index, newEmpire._empireId)
					print 'first empire at cell: ' + str(index._x) + '/' + str(index._y)
		newEmpire.updateState()

	def isEmpireBoundary(self, region):
		index = Point2DInt(region._x-1, region._y)
		if self.checkPosition(index):
			if self.getValue("id", index)!=self.getValue("id", region):
				return True
				
		index._x = region._x+1
		if self.checkPosition(index):
			if self.getValue("id", index)!=self.getValue("id", region):
				return True
				
		index._x = region._x
		index._y = region._y-1
		if self.checkPosition(index):
			if self.getValue("id", index)!=self.getValue("id", region):
				return True

		index._y = region._y+1
		if self.checkPosition(index):
			if self.getValue("id", index)!=self.getValue("id", region):
				return True
	
		return False

	def calculateAsabiya(self):
		index = Point2DInt(0,0)
		for index._x in range(0, self._config._size):
			for index._y in range(0, self._config._size):
				if self.isEmpireBoundary(index):
					asabiya = float(self.getValue("asabiya", index)/1000.0)
					newAsabiya = asabiya + self._config._asabiyaIncrease*asabiya*(1.0-asabiya)
					self.setValue("asabiya", index, int(1000.0*newAsabiya))
					#print '\tcell: ' + str(index._x) + "/" + str(index._y) + ' boundary had asabiya: ' + str(asabiya) + ' now: ' + str(self.getValue("asabiya", index)/1000.0)
				else:
					asabiya = float(self.getValue("asabiya", index)/1000.0)
					newAsabiya = asabiya - self._config._asabiyaDecay*asabiya
					# value must not be 0
					self.setValue("asabiya", index, max(1, int(1000.0*newAsabiya)))
					#print '\tcell: ' + str(index._x) + "/" + str(index._y)+ ' NOT boundary had asabiya: ' + str(asabiya) + ' now: ' + str(self.getValue("asabiya", index)/1000.0)

	def cleanEmpireRaster(self, empireId):
		index = Point2DInt(0,0)
		for index._x in range(0, self._config._size):
			for index._y in range(0, self._config._size):
				if self.getValue("id", index)==empireId:
					self.setValue("id", index, 0)


	def checkCollapse(self):
		index = Point2DInt(0,0)
		for index._x in range(0, self._config._size):
			for index._y in range(0, self._config._size):
				empireIds = self.getAgentIds(index, 'Empire')
				for empireId in empireIds:
					empire = self.getAgent(empireId)
					print "empire: " + empireId + " has average asabiya: " + str(empire._averageAsabiya)
					if empire._averageAsabiya<self._config._minAsabiya:
						print "empire: " + empireId + " collapses"
						self.cleanEmpireRaster(empire._empireId)
						empire._numRegions = 0
						self.removeAgent(empire)

	def attack(self, attacker, defender):
	
		# out of boundaries
		if not self.checkPosition(defender):
			return

		# same empire, not attack
		if self.getValue("id", attacker)!=0:
			if self.getValue("id", defender)==self.getValue("id", attacker):
				return
		
		numAttackingRegions = 1
		numDefendingRegions = 1
		averageAttackingAsabiya = self.getValue("asabiya", attacker)/1000.0
		averageDefendingAsabiya = self.getValue("asabiya", defender)/1000.0
		distToAttackingCentre = 0
		distToDefendingCentre = 0

		# attacker not empire
		if self.getValue("id", attacker)!=0:
			attackingEmpire = self.getAgent("Empire_"+str(self.getValue("id", attacker)))
			numAttackingRegions = attackingEmpire._numRegions
			averageAttackingAsabiya = attackingEmpire._averageAsabiya
			distToAttackingCentre = attackingEmpire.position.distance(attacker)

		if self.getValue("id", defender)!=0:
			defendingEmpire = self.getAgent("Empire_"+str(self.getValue("id", defender)))
			numDefendingRegions = defendingEmpire._numRegions
			averageDefendingAsabiya = defendingEmpire._averageAsabiya
			distToDefendingCentre = defendingEmpire.position.distance(defender)
	

		powerAttacker = numAttackingRegions*averageAttackingAsabiya*math.exp(-distToAttackingCentre/self._config._distanceSensibility)
		powerDefender = numDefendingRegions*averageDefendingAsabiya*math.exp(-distToDefendingCentre/self._config._distanceSensibility)
		
		print "\tattack from : " + str(attacker._x) + '/' + str(attacker._y) + ' to: ' +  str(defender._x) + '/' + str(defender._y) + " power att: " + str(powerAttacker) + " power def: " + str(powerDefender) + " diff: " + str(powerAttacker-powerDefender) + " thresh: " + str(self._config._attackThreshold)

		self.setValue("executed", attacker, 1)
		if(powerAttacker-powerDefender>self._config._attackThreshold):
			self.setValue("executed", defender, 1)
			# losing empire
			if self.getValue("id", defender)!=0:
				defendingEmpire = self.getAgent("Empire_"+str(self.getValue("id", defender)))
				defendingEmpire._numRegions -= 1
				print "\tdefending empire: " + str(defendingEmpire._empireId) + " lost cell"

			# new empire!
			if self.getValue("id", attacker)==0:
				newEmpire = Empire("Empire_"+str(self._numEmpires))
				newEmpire._averageAsabiya = self.getValue("asabiya", attacker)/1000.0
				newEmpire._numRegions = 2
				newEmpire._empireId = self._numEmpires

				self.setValue("id", attacker, newEmpire._empireId)
				self._numEmpires += 1
				
				self.addAgent(newEmpire)
				newEmpire.position = attacker
				
				self.setValue("id", defender, newEmpire._empireId)
				self.setValue("asabiya", defender, int(newEmpire._averageAsabiya*1000.0))
				print "\tcreation of new empire: " + str(newEmpire._empireId)

			else:
				attackingEmpire = self.getAgent("Empire_"+str(self.getValue("id", attacker)))
				attackingEmpire._numRegions += 1

				self.setValue("id", defender, attackingEmpire._empireId)
				self.setValue("asabiya", defender, int(attackingEmpire._averageAsabiya*1000.0))
				print "\tattacking empire: " + str(attackingEmpire._empireId) + " conquered cell"
		else:
			print "\tattack failed"


	def checkConflicts(self):
		# TODO conflicts must be randomized
		index = Point2DInt(0,0)
		potentialAttackers = []
		for index._x in range(0, self._config._size):
			for index._y in range(0, self._config._size):
				if self.isEmpireBoundary(index):
					potentialAttackers.append(Point2DInt(index._x, index._y))
					print "adding: " + str(index._x) + "/" + str(index._y)


		random.shuffle(potentialAttackers)
		for attacker in potentialAttackers:
			if self.getValue("executed", attacker)==0:			
				print 'checking attacks from: ' + str(attacker._x) + "/" + str(attacker._y)
				self.attack(attacker, Point2DInt(attacker._x-1, attacker._y))
				self.attack(attacker, Point2DInt(attacker._x+1, attacker._y))
				self.attack(attacker, Point2DInt(attacker._x, attacker._y-1))
				self.attack(attacker, Point2DInt(attacker._x, attacker._y+1))
		
		# clean executed raster
		index = Point2DInt(0,0)
		for index._x in range(0, self._config._size):
			for index._y in range(0, self._config._size):
				self.setValue("executed", index, 0)

	
	def stepEnvironment(self):
		self.calculateAsabiya()
		self.checkConflicts()
		#self.checkCollapse()

		return

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('-x', '--config', default='config.xml', help='config file')
	args = parser.parse_args()
	config = FrontierConfig()
	config.deserialize(args.config)

	print('size: '+str(config._size)+' num steps:'+str(config._numSteps))
	mySimulation = Simulation(config._size, config._numSteps)
	frontier = Frontier(mySimulation, config)
	frontier.initialize()
	frontier.run()
 
if __name__ == "__main__":
	main()

