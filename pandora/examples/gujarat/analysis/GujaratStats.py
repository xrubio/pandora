import os
import sys

def get_sim_length( agents ) :
	max_t = 0
	for a in agents :
		if len(a.time_steps) > max_t :
			max_t = len(a.time_steps)

	return max_t

class TimeStepData :
	
	def __init__( self, index ) :
		self.index = index
		self.action = None
		self.biomass_input = None
		self.cal_input = None
		self.isStarving = None
		self.balanceAfterAction = None

class ActivityLog :

	def __init__( self ) :
		self.yearly = {}
		self.yearlyCounts = 0
		self.seasonal = {}
		self.seasonalCounts = {}
		for i in [0, 1, 2 ] :
			self.seasonal[i] = {}
			self.seasonalCounts[i] = 0

	def process( self, t ) :

		if 'move_home' in t.action[0] :
			key = 'move_home'
		elif 'forage' in t.action[0] :
			key = 'forage'
		elif 'do_nothing' in t.action[0] :
			key = 'do_nothing'
		else :
			raise RuntimeError, "Unknown action %s found!"%t.action
		try :
			self.yearly[key] += 1
		except KeyError :
			self.yearly[key] = 1
		self.yearlyCounts += 1

		seasonIndex = (t.index % 360) / 120

		assert seasonIndex in [0,1,2]

		try :
			self.seasonal[seasonIndex][key] += 1
		except KeyError :
			self.seasonal[seasonIndex][key] = 1
		self.seasonalCounts[seasonIndex] += 1	

	def normalize( self, years ) :
		self.yearlyAvgDaysPerActivity = {}
		for key, value in self.yearly.iteritems() :
			self.yearlyAvgDaysPerActivity[ key ] = float(value)/float(self.yearlyCounts)
		self.seasonalAvgDaysPerActivity = {}
		for i in [0, 1, 2 ] :
			self.seasonalAvgDaysPerActivity[i] = {}
			self.seasonalAvgDaysPerActivity[i]['move_home'] = 0.0
			self.seasonalAvgDaysPerActivity[i]['forage'] = 0.0
			self.seasonalAvgDaysPerActivity[i]['do_nothing'] = 0.0
			for key, value in self.seasonal[i].iteritems() :
				self.seasonalAvgDaysPerActivity[ i ][key] = float(value) / float(self.seasonalCounts[i])



class NutritionLog :

	def __init__( self ) :
		self.yearlyStarvingCount = 0
		self.yearlyAvgBalance = 0.0
		self.yearlyAvgBiomassInput = 0.0
		self.yearlyAvgCalInput = 0.0
		self.yearlyCounts = 0
		self.yearlyForageCount = 0
		self.seasonalStarving = {}
		self.seasonalAvgBiomassInput = {}
		self.seasonalAvgCalInput = {}
		self.seasonalAvgBalance = {}
		self.seasonalCounts = {}
		self.seasonalForageCount = {}
		for i in [0, 1, 2 ] :
			self.seasonalStarving[i] = 0.0
			self.seasonalAvgBalance[i] = 0.0
			self.seasonalAvgBiomassInput[i] = 0.0
			self.seasonalAvgCalInput[i] = 0.0
			self.seasonalCounts[i] = 0
			self.seasonalForageCount[i] = 0
		

	def process( self, t ) :
	
		self.yearlyStarvingCount += t.isStarving
		self.yearlyAvgBalance += t.balanceAfterAction
		self.yearlyCounts += 1
		seasonIndex = (t.index % 360) / 120
		
		assert seasonIndex in [0,1,2]

		if 'forage' in t.action[0] :
			self.yearlyForageCount += 1
			self.yearlyAvgBiomassInput += float(t.action[1])
			self.yearlyAvgCalInput += float(t.action[2])
			self.seasonalForageCount[seasonIndex] += 1
			self.seasonalAvgBiomassInput[seasonIndex] += float(t.action[1])
			self.seasonalAvgCalInput[seasonIndex] += float(t.action[2])

		self.seasonalStarving[seasonIndex] += t.isStarving
		self.seasonalAvgBalance[seasonIndex] += float(t.balanceAfterAction)

		self.seasonalCounts[seasonIndex] += 1

	def 	normalize( self, years ) :
		self.yearlyStarvingCount /= float(years)
		self.yearlyAvgBalance = float(self.yearlyAvgBalance) / (float(self.yearlyCounts)*years)
		self.yearlyAvgBiomassInput /= float(self.yearlyForageCount)
		self.yearlyAvgCalInput /= float(self.yearlyForageCount)
		for i in [0,1,2] :
			self.seasonalStarving[i] /= float(years)
			self.seasonalAvgBalance[i] /= (float(self.seasonalCounts[i])*years)
			self.seasonalAvgBiomassInput[i] /= self.seasonalForageCount[i]
			self.seasonalAvgCalInput[i] /= self.seasonalForageCount[i]

class Agent :

	def __init__( self, log ) :
		if not os.path.exists( log ) :
			raise RuntimeError, "Cannot open log file: %s"%log
		self.name = None
		self.time_steps = []
		self.stats = {}
		self.__load( log )
		self.stats['activityLog'] = ActivityLog()
		self.stats['nutritionLog'] = NutritionLog()

	def __load( self, logfile ) :

		self.name = os.path.basename(logfile).split('.')[0]

		with open( logfile ) as instream :
			for line in instream :
				line = line.strip()
				key, value = line.split('=')
				if 'timestep' in key :
					t = TimeStepData( int(value) )
					self.time_steps.append( t )
					continue
				if 'action' in key :
					self.time_steps[-1].action = value.split(',')
					continue
				if 'isStarving' in key :
					if value == 'yes' :
						self.time_steps[-1].isStarving = 1
					else :
						self.time_steps[-1].isStarving = 0
					continue
				if 'collectedResourcesAfterConsumption' in key :
					self.time_steps[-1].balanceAfterAction = int(value)

	def computeStats( self, t0, tf ) :
		
		for statName, stat in self.stats.iteritems() :
			print >> sys.stdout, "Computing", statName, "statistics for agent", self.name
			for t in self.time_steps :
				if t.index >= t0 and t.index < tf :
					stat.process( t )
			years = len(self.time_steps)/(tf-t0)
			if years == 0 : years = 1
			stat.normalize( years )

class PopulationStats :

	def __init__( self, agentList ) :
		self.agents = agentList
		self.yearlyActivities = {}
		self.yearlyActivities['move_home'] = 0.0
		self.yearlyActivities['forage'] = 0.0
		self.yearlyActivities['do_nothing'] = 0.0
		self.seasonalActivities = {}
		for i in [0, 1, 2 ] :
			self.seasonalActivities[i] = {}
			self.seasonalActivities[i]['move_home'] = 0.0
			self.seasonalActivities[i]['forage'] = 0.0
			self.seasonalActivities[i]['do_nothing'] = 0.0

		self.yearlyStarvingCount = 0
		self.yearlyAvgBalance = 0.0
		self.yearlyAvgBiomassInput = 0.0
		self.yearlyAvgCalInput = 0.0
		self.yearlyMinStarvingCount = 360
		self.yearlyMaxStarvingCount = 0
		self.seasonalStarving = {}
		self.seasonalStarvingMin = {}
		self.seasonalStarvingMax = {}
		self.seasonalAvgBalance = {}
		self.seasonalAvgBiomassInput = {}
		self.seasonalAvgCalInput = {}
		for i in [0, 1, 2 ] :
			self.seasonalStarvingMin[i] = 360
			self.seasonalStarvingMax[i] = 0
			self.seasonalStarving[i] = 0.0
			self.seasonalAvgBalance[i] = 0.0
			self.seasonalAvgBiomassInput[i] = 0.0
			self.seasonalAvgCalInput[i] = 0.0

		self.__computeAggregateStats()
		self.__normalizeStats()

	def __computeAggregateStats( self ) :
		
		for agent in self.agents :
			for key, value in agent.stats['activityLog'].yearlyAvgDaysPerActivity.iteritems() :
				self.yearlyActivities[key] += float(value)
			self.yearlyStarvingCount += float(agent.stats['nutritionLog'].yearlyStarvingCount)
			self.yearlyAvgBalance += float(agent.stats['nutritionLog'].yearlyAvgBalance)
			if self.yearlyMinStarvingCount > agent.stats['nutritionLog'].yearlyStarvingCount :
				self.yearlyMinStarvingCount = agent.stats['nutritionLog'].yearlyStarvingCount
			if self.yearlyMaxStarvingCount < agent.stats['nutritionLog'].yearlyStarvingCount :
				self.yearlyMaxStarvingCount = agent.stats['nutritionLog'].yearlyStarvingCount

			self.yearlyAvgBiomassInput += float(agent.stats['nutritionLog'].yearlyAvgBiomassInput)
			self.yearlyAvgCalInput += float(agent.stats['nutritionLog'].yearlyAvgCalInput)
			
			for i in [0,1,2] :
				for key, value in agent.stats['activityLog'].seasonalAvgDaysPerActivity[i].iteritems() :
					self.seasonalActivities[i][key] += float(value)
				self.seasonalStarving[i] += float( agent.stats['nutritionLog'].seasonalStarving[i] )
				self.seasonalAvgBalance[i] += float( agent.stats['nutritionLog'].seasonalAvgBalance[i] )
				self.seasonalAvgBiomassInput[i] += float( agent.stats['nutritionLog'].seasonalAvgBiomassInput[i] )
				self.seasonalAvgCalInput[i] += float( agent.stats['nutritionLog'].seasonalAvgCalInput[i] )
				if self.seasonalStarvingMin[i] >  agent.stats['nutritionLog'].seasonalStarving[i] :
					self.seasonalStarvingMin[i] = agent.stats['nutritionLog'].seasonalStarving[i]
				if self.seasonalStarvingMax[i] < agent.stats['nutritionLog'].seasonalStarving[i] :
					self.seasonalStarvingMax[i] = agent.stats['nutritionLog'].seasonalStarving[i]

	def __normalizeStats( self ) :
		for key, value in self.yearlyActivities.iteritems() :
			self.yearlyActivities[key] /= float( len(self.agents) )
		self.yearlyStarvingCount /= float( len(self.agents) )
		self.yearlyAvgBalance /= float( len(self.agents) )
		self.yearlyAvgBiomassInput /= float( len(self.agents) )
		self.yearlyAvgCalInput /= float( len(self.agents) )
		for i in [0,1,2] :
			for key, value in self.seasonalActivities[i].iteritems() :
				self.seasonalActivities[i][key] /= float( len(self.agents) )
			self.seasonalStarving[i] /= float( len(self.agents) )
			self.seasonalAvgBalance[i] /= float( len(self.agents) )	
			self.seasonalAvgBiomassInput[i] /= float( len(self.agents) )
			self.seasonalAvgCalInput[i] /= float( len(self.agents) )

	def report( self, stream ) :

		print >> stream, "Population Statistics"
		print >> stream, "====================="
		print >> stream, "Yearly Time Distribution:"
		for key, value in self.yearlyActivities.iteritems() :
			print >> stream, "\t", key, "%.2f"%(value*100)
		print >> stream, "Seasonal Time Distribution:"
		for i in [0,1,2] :
			print >> stream, "\t", "Season #%d"%i
			for key, value in self.seasonalActivities[i].iteritems() :
				print >> stream, "\t\t", key, "%.2f"%(value*100)
		print >> stream, "Avg. Biomass collected per Forage action: %.2f grs"%self.yearlyAvgBiomassInput
		print >> stream, "Avg. Calories collected per Forage action: %.2f cal"%self.yearlyAvgCalInput
		print >> stream, "Avg. Number of days starving (per year) : %.2f days"%self.yearlyStarvingCount
		print >> stream, "Min. Number of days starving (per year) : %d days"%self.yearlyMinStarvingCount
		print >> stream, "Max. Number of days starving (per year) : %d days"%self.yearlyMaxStarvingCount
		
		print >> stream, "Avg. Balance (per year): %.2f cal"%self.yearlyAvgBalance
		print >> stream, "Seasonal Nutrition Facts:"
		for i in [0,1,2] :
			print >> stream, "\t", "Season #%d"%i
			print >> stream, "\t\t", "Avg. Number of days starving: %.2f days"%self.seasonalStarving[i]
			print >> stream, "\t\t", "Min Number of days starving: %d days"%self.seasonalStarvingMin[i]
			print >> stream, "\t\t", "Max Number of days starving: %d days"%self.seasonalStarvingMax[i]
			print >> stream, "\t\t", "Avg. Biomass collected per Forage action: %.2f grs"%self.seasonalAvgBiomassInput[i]
			print >> stream, "\t\t", "Avg. Calories collected per Forage action: %.2f cal"%self.seasonalAvgCalInput[i]
			print >> stream, "\t\t", "Avg. Balance: %.2f cal"%self.seasonalAvgBalance[i]

	
		
			
