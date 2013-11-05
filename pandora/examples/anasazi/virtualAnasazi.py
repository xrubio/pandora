#!/usr/bin/python

import sys
import random
import argparse
import math
import xml.etree.ElementTree

import time

sys.path.append('/home/xrubio/workspace/pandora/pandora/pyPandora/')
sys.path.append('/home/xrubio/workspace/pandora/pandora/')

from pyPandora import Simulation, Agent, World, Point2DInt

def enum(**enums):
	""" converts a sequence of values to an C++ style enum """
	return type('Enum', (), enums)

landCover = enum(eGeneralValleyFloor=0, eNorthValleyFloor=10, eNorthDunes=15, eMidValleyFloor=20, eMidDunes=25, eNonarableUplands=30, eArableUplands=40, eKinbikoCanyon=50, eEmpty=60)
terrain = enum(eGeneral=0, eNorth=1, eMid=2, eNatural=3, eUpland=4, eKinbiko=5)
apdsiValue = enum(eThree=0, eOne=1, eMinusOne=2, eMinusThree=3, eLessMinusThree=4)
maize = enum(eEmpty=0, eNoYield=1, eSandDune=2, eYield3=3, eYield2=4, eYield1=5)

class WaterLocation():
	def __init__(self):
		self._id = -1
		self._position = Point2DInt(-1, -1)
		self._typeWater = 0
		self._startDate = 0
		self._endDate = 0

	def checkAvailable(self, valley):
		if self._typeWater == 2:
			valley.setValue('water', self._position, 1)
			return
		if self._typeWater == 3 and valley._year>=self._startDate and valley._year<=self._endDate:
			valley.setValue('water', self._position, 1)
	
	def __str__(self):
		return 'WaterLocation: ' + str(self._id) + ' pos: ' + str(self._position._x) + '/' + str(self._position._y) + '\n\ttype: ' + str(self._typeWater) + ' date: ' + str(self._startDate) + '->' + str(self._endDate)

class HistoricalSettlement():
	def __init__(self):
		self._id = -1
		self._position = Point2DInt(-1, -1)
		self._startDate = 0
		self._endDate = 0
		self._medianDate = 0
		self._typeSettlement = 0
		self._size = 0
		self._description = 0
		self._roomCount = 0
		self._elevation = 0
		self._baselineHouseholds = 0

	def checkExists(self, valley):
		if valley._year<self._startDate or valley._year>=self._endDate:
			valley.setValue('historical', self._position, 0)
			return

		# in the netlogo model numHouseHolds = 0 with year = medianDate
		numHouseHolds = 0 #settlement._baselineHouseholds
		if valley._year > self._medianDate:
			numHouseHolds = math.ceil(float(self._baselineHouseholds) * float(self._endDate - valley._year)/float(self._endDate - self._medianDate))
			numHouseHolds = max(1.0, numHouseHolds)
			numHouseHolds = int(numHouseHolds)
		else:
			if self._medianDate!=self._startDate:
				numHouseHolds = math.ceil(float(self._baselineHouseholds) * float(valley._year - self._startDate)/float(self._medianDate - self._startDate))
				numHouseHolds = max(1.0, numHouseHolds)
				numHouseHolds = int(numHouseHolds)
		valley.setValue('historical', self._position, numHouseHolds)

	def __str__(self):
		return 'Settlement: ' + str(self._id) + ' pos: ' + str(self._position._x) + '/' + str(self._position._y) + '\n\tdate: ' + str(self._startDate) + '->' + str(self._endDate) + ' median: ' + str(self._medianDate) + '\n\ttype: ' + str(self._typeSettlement) + ' size: ' + str(self._size) + ' description ' + str(self._description) + ' room count: ' + str(self._roomCount) + ' elev: ' + str(self._elevation) + ' blhh: ' + str(self._baselineHouseholds)

class ValleyConfig():
	def __init__(self):
		self._size = Point2DInt(0,0)
		self._numSteps = 0

		self._landCover = 'unknown'
		self._settlements = 'unknown'
		self._hydro = 'unknown'
		self._apdsi = 'unknown'

		self._harvestVariance = 0.0
		self._harvestAdjustment = 0.0
		self._mobilityDist = 0
		
		self._initialNumber = 0

		self._nutritionNeed = 0 

		self._minInitialAge = 0
		self._maxInitialAge = 0
		self._minDeathAge = 0
		self._maxDeathAge = 0
	
		self._minFertility = 0.0
		self._maxFertility = 0.0
		self._minFertilityAge = 0
		self._maxFertilityAge = 0
		self._minFertilityEndsAge = 0
		self._maxFertilityEndsAge = 0

		self._maizeToGift = 0

		self._minInitialCorn = 0
		self._maxInitialCorn = 0

		self._locations = []

	def deserialize(self, xmlFile):
		tree = xml.etree.ElementTree.parse(xmlFile)
		root = tree.getroot()	
		
		self._resultsFile = str(root.find('output').get('resultsFile'))
		self._logsDir = str(root.find('output').get('logsDir'))

		self._size._x = int(root.find('size').get('width'))
		self._size._y = int(root.find('size').get('height'))
		self._numSteps = int(root.find('numSteps').get('value'))
		self._serializeResolution = int(root.find('numSteps').get('serializeResolution'))

		self._landCover = root.find('data').get('landCover')
		self._pdsi= root.find('data').get('pdsi')
		self._water = root.find('data').get('water')
		self._settlements = root.find('data').get('settlements')
		self._hydro = root.find('data').get('hydro')
		self._apdsi = root.find('data').get('apdsi')
	
		self._harvestVariance = float(root.find('environment').get('harvestVariance'))
		self._harvestAdjustment = float(root.find('environment').get('harvestAdjustment'))
		self._mobilityDist = int(root.find('environment').get('mobilityDist'))

		self._initialNumber = int(root.find('households').get('initialNumber'))
		
		typicalSize = int(root.find('households').get('typicalSize'))
		nutritionNeedPerPerson = int(root.find('households').get('nutritionNeedPerPerson'))
		self._nutritionNeed = typicalSize*nutritionNeedPerPerson
		
		self._maizeToGift = float(root.find('households').get('maizeToGift'))

		self._minInitialAge = int(root.find('households').find('initialAge').get('min'))
		self._maxInitialAge = int(root.find('households').find('initialAge').get('max'))	
		self._minDeathAge = int(root.find('households').find('deathAge').get('min'))
		self._maxDeathAge = int(root.find('households').find('deathAge').get('max'))	
	
		self._minFertility = float(root.find('households').find('fertility').get('min'))
		self._maxFertility = float(root.find('households').find('fertility').get('max'))
		self._minFertilityAge = int(root.find('households').find('fertilityAge').get('min'))
		self._maxFertilityAge = int(root.find('households').find('fertilityAge').get('max'))	
		self._minFertilityEndsAge = int(root.find('households').find('fertilityEndsAge').get('min'))
		self._maxFertilityEndsAge = int(root.find('households').find('fertilityEndsAge').get('max'))

		self._minInitialCorn = int(root.find('households').find('initialCorn').get('min'))
		self._maxInitialCorn = int(root.find('households').find('initialCorn').get('max'))	


class Household(Agent):
	def __init__(self, id, numId):
		Agent.__init__( self, id)
		
		self._age = 0
		self._deathAge = 0
		
		self._fertility = 0.0
		self._fertilityAge = 0
		self._fertilityEndsAge = 0
			
		self._lastHarvest = 0.0
		self._stock = []
		self._stock.append(0.0)
		self._stock.append(0.0)
		self._stock.append(0.0)
		self._estimates = 0.0

		self._bestFarm = Point2DInt(0,0)

	def getDistToWater(self, location):
		distFunction = location.distance
		distances = map(distFunction, self.getWorld()._waterSpots)
		return min(distances)

	def determineBestFarm(self):
		bestCandidates = []

		#print 'number of potential farms: ',len(self.getWorld()._potentialFarms)
		for farm in self.getWorld()._potentialFarms:
			# TODO potential farm can be a target
			if self.getWorld().getValue('farms', farm)!=0 and (self._bestFarm._x!=farm._x or self._bestFarm._y!=farm._y):
				continue
			if self.getWorld().getValue('households', farm)!=0:
			 	continue
			bestCandidates.append(farm)
			 
		#print 'number of candidates: ',len(bestCandidates)
		if len(bestCandidates)==0:
			return False

		bestDistance = 0
		random.shuffle(bestCandidates)
		bestCandidate = bestCandidates[0]
		for candidate in bestCandidates:
			distance = candidate.distance(self._bestFarm)
			if distance>bestDistance:
				continue
			bestDistance = distance
			bestCandidate = candidate

		self.getWorld().setValue('farms', self._bestFarm, 0)
		self._bestFarm._x = bestCandidate._x
		self._bestFarm._y = bestCandidate._y
		self.getWorld().setValue('farms', self._bestFarm, 1)
		self.getWorld()._potentialFarms.remove(bestCandidate)
		#print self,' chose best farm: ',self._bestFarm,' with yield: ',self.getWorld().getValue('yield x100', self._bestFarm)/100.0
		return True
	
	def findHouseholdSpot(self, closeToFarm, lowerYield ):
		candidate = self.position
		bestDistance = 1000	
		
		yieldFarmValue = self.getWorld().getValue('yield x100', self._bestFarm)
		for location in self.getWorld()._locations:		
			if closeToFarm==True and (location.distance(self._bestFarm)>self.getWorld()._config._mobilityDist):
				continue
			if lowerYield==True and self.getWorld().getValue('yield x100', location)>yieldFarmValue:
				continue
			if self.getWorld().getValue('hydro x100', location)>0:
				continue
			if self.getWorld().getValue('farms', location)!=0:
				continue
	
			distToWaterResources = self.getDistToWater(location)
			if distToWaterResources<bestDistance:	
				candidate._x = location._x
				candidate._y = location._y
				bestDistance = distToWaterResources
		
		# not available location
		if bestDistance==1000:
			return False

		self.getWorld().setValue('households', self.position, self.getWorld().getValue('households', self.position)-1)
		self.position._x = candidate._x
		self.position._y = candidate._y
		self.getWorld().setValue('households', self.position, self.getWorld().getValue('households', self.position)+1)
		self.getWorld().setValue('households', self.position, self.getWorld().getValue('households', self.position)+1)
		return True 

	def findSettlement(self):
		# first option no farm, close to farm and yield < farm yield
		if self.findHouseholdSpot(True, True):
			#print 'household found at first option: ',self
			return True
		# second option no farm, close to farm
		if self.findHouseholdSpot(True, False):
			#print 'household found at second option: ',self
			return True
		# third option, no farm
		if self.findHouseholdSpot(False, False):
			#print 'household found at third option: ',self
			return True
		# no location, die
		#print 'agent removed: ' + str(self)
		self.remove()
		return False

	def initHousehold(self, isAtInit=False):
		if isAtInit:
			self._age = random.randint(self.getWorld()._config._minInitialAge, self.getWorld()._config._maxInitialAge)
		else:
			self._age = 0
		self._deathAge = random.randint(self.getWorld()._config._minDeathAge, self.getWorld()._config._maxDeathAge)
		
		self._fertility = self.getWorld()._config._minFertility + random.random()*(self.getWorld()._config._maxFertility-self.getWorld()._config._minFertility)
		self._fertilityAge = random.randint(self.getWorld()._config._minFertilityAge, self.getWorld()._config._maxFertilityAge)
		self._fertilityEndsAge = random.randint(self.getWorld()._config._minFertilityEndsAge, self.getWorld()._config._maxFertilityEndsAge)

		self._bestFarm = Point2DInt(random.randint(0, self.getWorld()._config._size._x-1), random.randint(0, self.getWorld()._config._size._y-1))
		self.position = Point2DInt(random.randint(0, self.getWorld()._config._size._x-1), random.randint(0, self.getWorld()._config._size._y-1))
		self.getWorld().setValue('farms', self._bestFarm, 1)
		self.getWorld().setValue('households', self.position, self.getWorld().getValue('households', self.position)+1)
			
		if not self.determineBestFarm() or not self.findSettlement():
			self.removeAndClean()
			return
		if not isAtInit:
			return

		for i in xrange(0, 3):
			self._stock[i] = random.randrange(self.getWorld()._config._minInitialCorn, self.getWorld()._config._maxInitialCorn )
		self._lastHarvest = self._stock[0]
		
	def registerAttributes(self):
		"""
		self.registerIntAttribute('age')
		self.registerIntAttribute('deathAge')
		self.registerIntAttribute('fertility x100')
		self.registerIntAttribute('fertilityAge')
		self.registerIntAttribute('fertilityEndsAge')
		self.registerIntAttribute('lastHarvest')
		self.registerIntAttribute('estimates')
		"""
		return

	def serialize(self):
		"""
		self.serializeIntAttribute('age', self._age)
		self.serializeIntAttribute('deathAge', self._deathAge)
		self.serializeIntAttribute('fertility x100', int(self._fertility*100.0))
		self.serializeIntAttribute('fertilityAge', self._fertilityAge)
		self.serializeIntAttribute('fertilityEndsAge', self._fertilityEndsAge)
		self.serializeIntAttribute('lastHarvest', int(self._lastHarvest))
		self.serializeIntAttribute('estimates', int(self._lastHarvest))
		"""
		return

	def harvestConsumption(self):
		self._lastHarvest = float(self.getWorld().getValue('yield x100', self._bestFarm))/100.0
		# ALERT harvestVariance is applied two times to yield
		# ALERT last harvest can be negative
		self._lastHarvest = self._lastHarvest * (1.0+(random.normalvariate(0, 1)*float(self.getWorld()._config._harvestVariance)))
		if self._lastHarvest<0:
			self._lastHarvest = 0
		self._stock[2] = self._stock[1]	
		self._stock[1] = self._stock[0]
		self._stock[0] = self._lastHarvest
		#print self,' at year: ',self.getWorld()._year,' farm: ',self._bestFarm,' harvested: ',self._lastHarvest,'- stock 1st year: ',self._stock[1],' 2nd year:',self._stock[2],' sum:',sum(self._stock)

		covered  = self.getWorld()._config._nutritionNeed
		for i in xrange(len(self._stock)-1, -1, -1):
			if self._stock[i] >= covered:
				self._stock[i] = self._stock[i] - covered
				#print self,' after consumption, current stock: ',self._stock[0],' stock 1st year: ',self._stock[1],' 2nd year:',self._stock[2],' sum:',sum(self._stock)
				return
			else:
				covered = covered - self._stock[i]
				self._stock[i] = 0
		#print self,' after consumption, current stock: ',self._stock[0],' 1st year: ',self._stock[1],' 2nd year:',self._stock[2],' sum:',sum(self._stock)
	
	def checkDeath(self):
		# all stock consumed and still needs not covered
		if self._stock[0] == 0 and self._stock[1] == 0 and self._stock[2] == 0:
			return True
		if self._age >= self._deathAge:
			return True
		return False

	def estimateHarvest(self):
		self._estimate = self._lastHarvest + self._stock[0] + self._stock[1] # 2nd year if lost
		if self._estimate < self.getWorld()._config._nutritionNeed:
			#print 'estimate: ',self._estimate,' bad needed: ',self.getWorld()._config._nutritionNeed
			return False
		#print 'estimate: ',self._estimate,' good needed: ',self.getWorld()._config._nutritionNeed
		return True

	def checkFission(self):	
		if len(self.getWorld()._potentialFarms)==0:
			return
		if self._age < self._fertilityAge:
			return
		if self._age >= self._fertilityEndsAge:
			return
		if random.random() > self._fertility:
			return

		child = Household('Household_'+str(self.getWorld()._numIds), self.getWorld()._numIds)
		self.getWorld()._numIds = self.getWorld()._numIds + 1
		self.getWorld().addAgent(child)
		child.initHousehold(False)
		for i in xrange(0, len(self._stock)):
			child._stock[i] = self._stock[i]*self.getWorld()._config._maizeToGift
			self._stock[i] = self._stock[i]-child._stock[i]
		#print self,' had child: ',child,' remaining:',sum(self._stock),' child stock:',sum(child._stock)
	
	def removeAndClean(self):
		self.getWorld().setValue('farms', self._bestFarm, 0)
		self.getWorld().setValue('households', self.position, self.getWorld().getValue('households', self.position)-1)
		self.remove()
	
	def updateState(self):
		#print self,' executing year:',self.getWorld()._year
		self.harvestConsumption()
		if self.checkDeath():
			self.removeAndClean()
			return
		if not self.estimateHarvest():	
			if not self.determineBestFarm() or not self.findSettlement():
				self.removeAndClean()
				return
		self.checkFission()
		self._age = self._age+1

class Valley(World):

	def __init__(self, simulation, config ):
		World.__init__( self, simulation, 1, 1, config._resultsFile)
		self._config = config
		self._alluviumExist = False
		self._streamExist = False
		self._waterLocations = []
		self._waterStaticLocations = []
		self._historicalSettlements = []
		self._apdsi = []
		self._hydro = []
		self._year = 800
		self._numIds = 0

		# efficiency variables
		self._locations = []
		self._potentialFarms = []
		self._waterSpots = []

	def loadLandCover(self):
		print 'loading: ' + self._config._landCover
		f = open(self._config._landCover, 'r')
		line = f.readline().split(' ')
		index = 0

		for i in xrange(0, self._config._size._x):
			for j in xrange(0, self._config._size._y):
				value = int(line[index])
				maizeZone = maize.eEmpty
				
				if value == landCover.eGeneralValleyFloor:
					maizeZone = maize.eYield2
				if value == landCover.eNorthValleyFloor:
					maizeZone = maize.eYield1
				if value == landCover.eNorthDunes:
					maizeZone = maize.eSandDune
				if value == landCover.eMidValleyFloor:
					if i<=74:
						maizeZone = maize.eYield1		
					else:
						maizeZone = maize.eYield2
				if value == landCover.eMidDunes:
					maizeZone = maize.eSandDune
				if value == landCover.eNonarableUplands:
					maizeZone = maize.eNoYield
				if value == landCover.eArableUplands:
					maizeZone = maize.eYield3
				if value == landCover.eKinbikoCanyon:
					maizeZone = maize.eYield1
				if value == landCover.eEmpty:
					maizeZone = maize.eEmpty
				
				self.setValue('landCover', Point2DInt(i,j), value)
				self.setValue('maize', Point2DInt(i,j), maizeZone)

				index = index+1	
		f.close()

	def transformCoord( self, x, y ):
		xCell = 24.5 + int((float(x)-2392.0)/93.5)
		if xCell-math.floor(xCell)>=0.5:
			xCell = int(math.ceil(xCell))
		else:
			xCell = int(math.floor(xCell))

		yCell = 45.0 + int(37.6 +  (float(y)-7954.0)/93.5)
		if yCell-math.floor(yCell)>=0.5:
			yCell = int(math.ceil(yCell))
		else:
			yCell = int(math.floor(yCell))
		yCell = 119 - yCell
		return Point2DInt(xCell, yCell)

	def loadWater(self):
		print 'loading: ' + self._config._water
		f = open(self._config._water, 'r')	
		line = f.readline().split(' ')
		index = 0
		for i in xrange(0, 108):
			x = int(line[index+2])
			y = int(line[index+1])

			if x==0 and y==0:
				index = index + 6
				continue
					
			water = WaterLocation()
			water._id = int(line[index])
			water._position = self.transformCoord(x, y)
			water._typeWater = int(line[index+3])
			if water._typeWater!=2 and water._typeWater!=3:
				index = index + 6
				continue
			water._startDate = int(line[index+4])
			water._endDate = int(line[index+5])
			self._waterLocations.append(water)

			index = index + 6	
		f.close()
		# predefined values
		self._waterStaticLocations.append(Point2DInt(72, 5))
		self._waterStaticLocations.append(Point2DInt(70, 6))
		self._waterStaticLocations.append(Point2DInt(69, 7))
		self._waterStaticLocations.append(Point2DInt(68, 8))
		self._waterStaticLocations.append(Point2DInt(67, 9))
		self._waterStaticLocations.append(Point2DInt(66, 10))
		self._waterStaticLocations.append(Point2DInt(65, 11))
		self._waterStaticLocations.append(Point2DInt(65, 12))
	
	def loadHistoricalSettlements(self):
		print 'loading: ' + self._config._settlements
		f = open(self._config._settlements, 'r')
		line = f.readline().split(' ')
		index = 0
		for i in xrange(0, 488):
			settlement = HistoricalSettlement()
			settlement._id = int(line[index])
			
			x = int(line[index+2])
			y = int(line[index+1])
			settlement._position = self.transformCoord(x, y)
			settlement._startDate = int(line[index+3])
			settlement._endDate = int(line[index+4])
			settlement._medianDate = 1950-int(line[index+5])
			settlement._typeSettlement = int(line[index+6])

			if settlement._typeSettlement!=1:
				index = index + 12
				continue
			
			settlement._size = int(line[index+7])
			settlement._description = int(line[index+8])
			settlement._roomCount = int(line[index+9])
			settlement._elevation = int(line[index+10])
			settlement._baselineHouseholds = int(line[index+11])
			self._historicalSettlements.append(settlement)

			index = index + 12	
		f.close()

	def getApdsi( self, value ):
		if value >= 3.0:
			return apdsiValue.eThree
		if value >= 1.0:
			return apdsiValue.eOne
		if value >=-1.0:
			return apdsiValue.eMinusOne
		if value>= -3.0:
			return apdsiValue.eMinusThree
		return apdsiValue.eLessMinusThree

	def loadHydro(self):
		print 'loading: ' + self._config._hydro
		f = open(self._config._hydro, 'r')
		line = f.readline().split(' ')
		for i in xrange(800, 801+self._config._numSteps):
			hydroStep = []
			for j in xrange(0, 6):				
				hydroStep.append(0)
			index = 15*(i - 382)
			hydroStep[terrain.eGeneral] = float(line[index+1])
			hydroStep[terrain.eNorth] = float(line[index+4])
			hydroStep[terrain.eMid] = float(line[index+7])
			hydroStep[terrain.eNatural] = float(line[index+10])
			hydroStep[terrain.eUpland] = float(line[index+10])
			hydroStep[terrain.eKinbiko] = float(line[index+13])
			self._hydro.append(hydroStep)
		
		f.close()
	
	def loadApdsi(self):
		print 'loading: ' + self._config._apdsi
		f = open(self._config._apdsi, 'r')
		line = f.readline().split(' ')
		for i in xrange(800, 801+self._config._numSteps):
			apdsiStep = []
			for j in xrange(0, 6):
				apdsiStep.append(0)
			apdsiStep[terrain.eGeneral] = self.getApdsi(float(line[i-200]))
			apdsiStep[terrain.eNorth] = self.getApdsi(float(line[1100+i]))
			apdsiStep[terrain.eMid] = self.getApdsi(float(line[2400+i]))
			apdsiStep[terrain.eNatural] = self.getApdsi(float(line[3700+i]))
			apdsiStep[terrain.eUpland] = self.getApdsi(float(line[3700+i]))
			apdsiStep[terrain.eKinbiko] = self.getApdsi(float(line[1100+i]))
			# ALERT THERE IS AN ERROR IN NETLOGO MODEL: EVALUATION IS MADE OF UPLANDS WITH UPLAND, SO IT IS ALWAYS 0
			self._apdsi.append(apdsiStep)

			self._yieldTable = []
			for i in xrange(0, 1+maize.eYield1):	
				yieldEntry = []
				for j in xrange(0, 1+apdsiValue.eLessMinusThree):
					yieldEntry.append(0)
				if i==maize.eSandDune:
				 	yieldEntry[apdsiValue.eThree]=1201
				 	yieldEntry[apdsiValue.eOne]=1030
				 	yieldEntry[apdsiValue.eMinusOne]=855
				 	yieldEntry[apdsiValue.eMinusThree]=749
				 	yieldEntry[apdsiValue.eLessMinusThree]=672
	
				if i==maize.eYield3:
				 	yieldEntry[apdsiValue.eThree]=769
				 	yieldEntry[apdsiValue.eOne]=659
				 	yieldEntry[apdsiValue.eMinusOne]=547
				 	yieldEntry[apdsiValue.eMinusThree]=479
				 	yieldEntry[apdsiValue.eLessMinusThree]=411
							
				if i==maize.eYield2:
				 	yieldEntry[apdsiValue.eThree]=961
				 	yieldEntry[apdsiValue.eOne]=824
				 	yieldEntry[apdsiValue.eMinusOne]=684
				 	yieldEntry[apdsiValue.eMinusThree]=599
				 	yieldEntry[apdsiValue.eLessMinusThree]=514
							
				if i==maize.eYield1:
				 	yieldEntry[apdsiValue.eThree]=1153
				 	yieldEntry[apdsiValue.eOne]=988
				 	yieldEntry[apdsiValue.eMinusOne]=821
				 	yieldEntry[apdsiValue.eMinusThree]=719
				 	yieldEntry[apdsiValue.eLessMinusThree]=617

				self._yieldTable.append(yieldEntry)
				
		f.close()

	def loadData(self):
		
		for i in xrange(0, self._config._size._x):
			for j in xrange(0, self._config._size._y):
				self._locations.append(Point2DInt(i, j))

		[self.fillQuality(location) for location in self._locations]

		self.loadLandCover()
		self.loadWater()
		self.loadApdsi()
		self.loadHydro()
		self.loadHistoricalSettlements()

		self._year = self.currentStep+800
		self.checkAlluvium()
		self.checkStream()
		self.stepYield()
		self.stepHistoricalSettlement()
		self.stepWater()
		
		self.stepWaterSpots()
		self.stepPotentialFarms()

	def fillQuality(self, location):
		quality = 1.0 + random.normalvariate(0, 1)*self._config._harvestVariance
		if quality < 0.0:
			quality = 0
		self.setValue('quality x100', location, int(quality*100.0))

	def createRasters(self):
		self.registerDynamicRaster("landCover", False)
		self.registerDynamicRaster("water", False)
		self.registerDynamicRaster("historical", False)
		self.registerDynamicRaster("farms", False)
		self.registerDynamicRaster("households", False)
		self.registerDynamicRaster("yield x100", False)
		self.registerDynamicRaster("quality x100", False)
		self.registerDynamicRaster("maize", False) 
		self.registerDynamicRaster("hydro x100", False)
		
		self.getDynamicRaster("landCover").setInitValues(0, 60, 60)
		self.getDynamicRaster("water").setInitValues(0, 1, 0)
		self.getDynamicRaster("historical").setInitValues(0, 10000, 0)
		self.getDynamicRaster("farms").setInitValues(0, 10000, 0)
		self.getDynamicRaster("households").setInitValues(0, 10000, 0)
		self.getDynamicRaster("yield x100").setInitValues(0, 1000000, 0)
		self.getDynamicRaster("maize").setInitValues(maize.eEmpty, maize.eYield1, maize.eEmpty)
		self.getDynamicRaster("quality x100").setInitValues(0, 10000, 0)
		self.getDynamicRaster("hydro x100").setInitValues(0, 1000000, 0)

		self.loadData()

	def createAgents(self):
		for i in xrange (1, 1+self._config._initialNumber):
			newAgent = Household('Household_'+str(i), i)
			self.addAgent(newAgent)
			newAgent.initHousehold(True)
		self._numIds = self._config._initialNumber+1

	def checkStream(self):
		self._streamExist = False

		if(self._year>=280 and self._year<360):
			self._streamExist = True
			return

		if(self._year>=800 and self._year<930):
			self._streamExist = True
			return

		if(self._year>=1300 and self._year<1450):
			self._streamExist = True

	def checkAlluvium(self):
		self._alluviumExist = False
		if(self._year>=420 and self._year<560):
			self._alluviumExist = True
			return

		if(self._year>=630 and self._year<680):
			self._alluviumExist = True
			return

		if(self._year>=980 and self._year<1120):
			self._alluviumExist = True
			return

		if(self._year>=1180 and self._year < 1230):
			self._alluviumExist = True

	def stepHistoricalSettlement(self):
		# clean	
		[settlement.checkExists(self) for settlement in self._historicalSettlements]

	def stepWater(self):
		# clean	
		def resetWater(self, location):
			landCoverValue = self.getValue('landCover', location)
			hydroIndex = self.getTerrainFromLandCover(landCoverValue)
			hydroValue = self._hydro[self.currentStep][hydroIndex]
			self.setValue('hydro x100', location, int(hydroValue*100.0))
			self.setValue('water', location, 0)
		[resetWater(self, location) for location in self._locations]
		# temporary locations
		[waterLocation.checkAvailable(self) for waterLocation in self._waterLocations]
		# static locations
		[self.setValue('water', spot, 1) for spot in self._waterStaticLocations]

		# landcover based spots
		if self._alluviumExist == True:
			def setAlluvium(self, location):
				cover = self.getValue('landCover', location)					
				if cover==landCover.eGeneralValleyFloor:
					self.setValue('water', location, 1)
				elif cover==landCover.eNorthValleyFloor:
					self.setValue('water', location, 1)
				elif cover==landCover.eMidValleyFloor:
					self.setValue('water', location, 1)
				elif cover==landCover.eKinbikoCanyon:
						self.setValue('water', location, 1)
			[setAlluvium(self, location) for location in self._locations]
		
		if self._streamExist == True:
			def setStream(self, location):
				cover = self.getValue('landCover', location)
				if cover==landCover.eKinbikoCanyon:
					self.setValue('water', location, 1)
			[setStream(self, location) for location in self._locations]

	def getTerrainFromLandCover( self, value ):
		# what if value==landCovereNorthDunes or value==landCover.eMidDunes?????
		if value==landCover.eGeneralValleyFloor:
			return terrain.eGeneral
		if value==landCover.eNorthValleyFloor or value==landCover.eNorthDunes:
			return terrain.eNorth
		if value==landCover.eMidValleyFloor or value==landCover.eMidDunes:
			return terrain.eMid
		if value==landCover.eNonarableUplands:
			return terrain.eNatural
		if value==landCover.eArableUplands:
			return terrain.eUpland
		if value==landCover.eKinbikoCanyon:
			return terrain.eKinbiko
		return -1


	def stepYield(self):
		def calculateYield(self, location):
			landCoverValue = self.getValue('landCover', location)
			if landCoverValue==landCover.eEmpty:
				return

			apdsiValue = self._apdsi[self.currentStep][self.getTerrainFromLandCover(landCoverValue)]
			# if no change use value of last time step
			if self.currentStep>0 and apdsiValue==self._apdsi[self.currentStep-1][self.getTerrainFromLandCover(landCoverValue)]:
				return
			maizeValue = self.getValue('maize', location)
			yieldValue = 0
			qualityValue = float(self.getValue('quality x100', location))/100.0
			yieldValue = self._yieldTable[maizeValue][apdsiValue]
			yieldValue = float(yieldValue) * qualityValue
			yieldValue = yieldValue * self._config._harvestAdjustment
			self.setValue('yield x100', location, int(100.0*yieldValue))	
		[calculateYield(self, location) for location in self._locations]

	def closeToWater(self, location):
		for spot in self._waterSpots:
			if spot.distance(location)<=self._config._mobilityDist:
				return True
		return False
	
	def stepWaterSpots(self):
		self._waterSpots[:] = []
		def checkPotentialWaterSpots(self, location):
			if self.getValue('water', location)==1:
				self._waterSpots.append(Point2DInt(location._x, location._y))
		[checkPotentialWaterSpots(self, location) for location in self._locations]

	def stepPotentialFarms(self):
		self._potentialFarms[:] = []
		def checkPotentialFarm(self, location):
			yieldValue = int(float(self.getValue('yield x100', location))/100.0)
			if yieldValue < self._config._nutritionNeed:
				return
			# already used
			if self.getValue('farms', location)==1:
				return
			if not self.closeToWater(location):
				return
			self._potentialFarms.append(Point2DInt(location._x, location._y))
		[checkPotentialFarm(self, location) for location in self._locations]
		#print 'number of potential farms in year: ',self._year,' is: ',len(self._potentialFarms)

	def stepEnvironment(self):
		self._year = self.currentStep+800
		self.checkAlluvium()
		self.checkStream()
		self.stepWater()
		self.stepYield()
		self.stepHistoricalSettlement()

		# update dynamic lists
		self.stepWaterSpots()
		self.stepPotentialFarms()
		return

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('-x', '--config', default='config.xml', help='config file')
	args = parser.parse_args()
	config = ValleyConfig()
	config.deserialize(args.config)

	mySimulation = Simulation(config._size, config._numSteps, config._serializeResolution)
	valley = Valley(mySimulation, config)
	valley.initialize()
	valley.run()


#import profile
if __name__ == "__main__":
#	profile.run('main()')i
	main()
