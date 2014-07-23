#!/usr/bin/env python3

import os, sys
from subprocess import call

pandoraPath = os.getenv('PANDORAPATH', '/usr/local/pandora')
sys.path.append(pandoraPath+'/bin')
sys.path.append(pandoraPath+'/lib')

import random
import argparse

from pyPandora import Config, Agent, World, Point2DInt, SizeInt

def enum(**enums):
    """ converts a sequence of values to an C++ style enum """
    return type('Enum', (), enums)

terrainTypes = enum(ePastures=0, eFields=1)

class OasisConfig(Config):
    def __init__(self, xmlFile):
        Config.__init__(self, xmlFile)

        # farmer config
        self._initFarmers = 0
        self._farmersStrength = 0.0
        self._farmersGrowthRate = 0.0
        self._farmersImmigrationRate = 0.0

        self._initHerders = 0
        self._herdersRelativeStrength = 0.0
        self._herdersGrowthRate = 0.0
        self._herdersImmigrationRate = 0.0
    
    def loadParams(self):
        self._initFarmers = self.getParamInt('agents/farmers', 'init')
        self._farmersStrength = self.getParamFloat('agents/farmers', 'strength')
        self._farmersGrowthRate = self.getParamFloat('agents/farmers', 'growthRate')
        self._farmersImmigrationRate = self.getParamFloat('agents/farmers', 'immigrationRate')

        self._initHerders = self.getParamInt('agents/herders', 'init')
        self._herdersRelativeStrength = self.getParamFloat('agents/herders', 'relativeStrength')
        self._herdersGrowthRate = self.getParamFloat('agents/herders', 'growthRate')
        self._herdersImmigrationRate = self.getParamFloat('agents/herders', 'immigrationRate')

class OasisAgent(Agent):
    """ Base class for the Musical Chairs model """
    def __init__(self, id):
        Agent.__init__( self, id)
        self._aggressiveness = 1.0
        self._strength = 1.0
        self._naturalGrowth = 0.0

    def naturalGrowth(self):
        # check natural growth
        randomValue = random.random()
        if randomValue > self._naturalGrowth:
            return
        newAgent = self.duplicateAgent()
        if newAgent == 0:
            return
        newAgent._strength = self._strength
        newAgent._aggressiveness = self._aggressiveness
        newAgent._naturalGrowth = self._naturalGrowth

    def registerAttributes(self):
        self.registerIntAttribute('strength x100')
        self.registerIntAttribute('aggressiveness x100')

    def serialize(self):
        self.serializeIntAttribute('strength x100', int(100.0*self._strength))
        self.serializeIntAttribute('aggressiveness x100', int(100*self._aggressiveness))
    
class Herder(OasisAgent):
    def __init__(self, id):
        OasisAgent.__init__( self, id)

    def updateState(self):
        if self.exists == False:
            return
        self.naturalGrowth()

    def getNewHerderPosition(self):
        for j in range(self.getWorld().config.size._width-1, -1, -1):
            for i in range(self.getWorld().config.size._height-1, -1, -1):
                possiblePosition = Point2DInt(i,j)
                if self.getWorld().getValue('herders', possiblePosition)==0:
                    return possiblePosition
        return Point2DInt(-1,-1) 

    def duplicateAgent(self):
        newAgent = Herder('Herder_'+str(self.getWorld()._numHerders))
        self.getWorld()._numHerders += 1
        newAgent.position = self.getNewHerderPosition()
        # all occupied by farmers
        if(newAgent.position._x == -1):
            return 0
        self.getWorld().setValue('herders', newAgent.position, 1) 
        self.getWorld().addAgent(newAgent)
        return newAgent
         
class Farmer(OasisAgent):
    def __init__(self, id):
        OasisAgent.__init__( self, id)

    def updateState(self):  
        if self.exists == False:
            return
        self.naturalGrowth()

    def getNewFarmerPosition(self):
        for j in range(0,self.getWorld().config.size._width):
            for i in range(0,self.getWorld().config.size._height):
                possiblePosition = Point2DInt(i,j)
                if self.getWorld().getValue('farmers', possiblePosition)==0:
                    return possiblePosition
        return Point2DInt(-1,-1) 

    def duplicateAgent(self):
        newAgent = Farmer('Farmer_'+str(self.getWorld()._numFarmers))
        self.getWorld()._numFarmers += 1
        newAgent.position = self.getNewFarmerPosition()
        # all occupied by farmers
        if(newAgent.position._x == -1):
            return 0
        self.getWorld().setValue('farmers', newAgent.position, 1) 
        self.getWorld().addAgent(newAgent)
        return newAgent

class Oasis(World):
    def __init__(self, config ):
        World.__init__( self, config, Oasis.useOpenMPSingleNode())
        self._numFields = 0
        self._herdersOut = 0
        self._farmersOut = 0
        self._aggressions = 0
        self._invasions = 0

        self._numFarmers = 0
        self._numHerders = 0

    def createRasters(self):
        print('aa')
        self.registerDynamicRaster("farmers", 1)
        self.registerDynamicRaster("herders", 1)
        self.registerDynamicRaster("conflicts", 1)
        self.getDynamicRaster("farmers").setInitValues(0, 1, 0)
        self.getDynamicRaster("herders").setInitValues(0, 1, 0)
        self.getDynamicRaster("conflicts").setInitValues(0, self.config.numSteps, 0)
        print('end aa')

    def createAgents(self):
        print('bb')
        print('creating farmers:',self.config._initFarmers,'and herders:',self.config._initHerders)
        for i in range (0, self.config._initFarmers):
            newAgent = Farmer('Farmer_'+str(i))
            self.addAgent(newAgent)
            newAgent._naturalGrowth = self.config._farmersGrowthRate
            newAgent._strength = self.config._farmersStrength
            newAgent.position = Point2DInt(int(i%self.config.size._width),int(i/self.config.size._height))
            self._numFarmers += 1
            self.setValue('farmers', newAgent.position, 1) 

        for i in range (0, self.config._initHerders):
            newAgent = Herder('Herder_'+str(i))
            self.addAgent(newAgent)
            newAgent._naturalGrowth = self.config._herdersGrowthRate
            newAgent._strength = self.config._farmersStrength*self.config._herdersRelativeStrength
            newAgent.position = Point2DInt(int(self.config.size._width-1-i%self.config.size._height),int(self.config.size._width-1-i/self.config.size._height))
            self._numHerders += 1
            self.setValue('herders', newAgent.position, 1) 
        print('end bb')
    
    def farmersMigration(self):
        return  
            
    def herdersMigration(self):
        return
    
    def resolveConflicts( self, position ):
        print('resolving conflict at position:',position)
        # just one of each per cell
        herder = self.getAgent(self.getAgentIds(position, 'Herder')[0])
        farmer = self.getAgent(self.getAgentIds(position, 'Farmer')[0])
        print('conflict between herder:',herder,'and farmer:',farmer,'at pos:',position)
        self.setValue('conflicts', position, self.getValue('conflicts', position)+1)

        indexOfOpportunity = self._numFields / (self.config.size._width*self.config.size._height)
        ratioOfStrengths = herder._strength/(herder._strength+farmer._strength)
        incentiveForMigration = 1 - ratioOfStrengths*indexOfOpportunity
        print('\tratio of strength:',ratioOfStrengths,'incentive migration:',incentiveForMigration,' index of opportunity: ',indexOfOpportunity,'num fields:',self._numFields,'size:',self.config.size)

        if incentiveForMigration>herder._aggressiveness:
            print('\therder migrated, removing agent: ',herder)
            herder.remove()
            self.setValue('herders', position, 0)
            self._herdersOut += 1
            return

        # invasion attempt
        self._aggressions += 1
        randomValue = random.random()
        if herder._aggressiveness > randomValue:
            print('\therder invasion success with random:',randomValue,'and aggressiveness:',herder._aggressiveness,'farmer removed:',farmer)
            farmer.remove()
            self.setValue('farmers', position, 0)
            self._invasions += 1
            return
        else:
            print('\therder invasion failed with random:',randomValue,'and aggressiveness:',herder._aggressiveness,'herder removed:',herder)
            herder.remove()
            self.setValue('herders', position, 0)
            return
    
    def updateNumFields(self):
        self._numFields = 0
        for i in range(0,self.config.size._width):
            for j in range(0,self.config.size._height):
                position = Point2DInt(i,j)
                if self.getValue('farmers', position)==1 :
                    self._numFields += 1
                
    def checkConflicts(self):
        self._aggressions = 0
        self._invasions = 0
        index = Point2DInt(0,0)
        for index._y in range(0,self.config.size._width):
            for index._x in range(0,self.config.size._height):
                if self.getValue('herders', index)==1 and self.getValue('farmers', index)==1:
                    self.resolveConflicts(index)
    
    def shufflePositions(self):
        """We need to shuffle the positions of the agents before checking conflicts"""
        listFarmers = []
        index = Point2DInt(0,0)
        for index._y in range(0,self.config.size._width):
            for index._x in range(0,self.config.size._height):
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
            if(index._x<(self.config.size._width-1)):
                index._x +=1
            # next row
            else:
                index._x = 0
                index._y += 1   
    
        listHerders = []
        index = Point2DInt(0,0) 
        for index._y in range(self.config.size._height-1, -1, -1):
            for index._x in range(self.config.size._width-1, -1, -1):
                if self.getValue('herders', index)==1:
                    aHerder = self.getAgent(self.getAgentIds(index, 'Herder')[0])
                    self.setValue('herders', index, 0)
                    listHerders.append(aHerder)

        random.shuffle(listHerders)
        index = Point2DInt(self.config.size._width-1,self.config.size._height-1)
        for i in range(0, len(listHerders)):
            aHerder= listHerders[i]
            aHerder.position = index
            self.setValue('herders', index, 1)
            # next column
            if index._x > 0:
                index._x -=1
            # next row
            else:
                index._x = self.config.size._width-1
                index._y -= 1


    def stepEnvironment(self):
        self.farmersMigration()
        self.herdersMigration()

        self.updateNumFields()

        self.shufflePositions()
        self.checkConflicts()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-x', '--config', default='config.xml', help='config file')
    args = parser.parse_args()

    config = OasisConfig(args.config)
    oasis = Oasis(config)
    oasis.initialize()
    oasis.run()
 
if __name__ == "__main__":
    main()

