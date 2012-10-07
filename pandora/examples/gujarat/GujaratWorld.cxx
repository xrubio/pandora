
#include <GujaratWorld.hxx>

#include <Raster.hxx>
#include <Point2D.hxx>
#include <HunterGatherer.hxx>
#include <AgroPastoralist.hxx>
#include <Exceptions.hxx>
#include <GujaratConfig.hxx>
#include <OriginalDemographics.hxx>
#include <RamirezDemographics.hxx>
#include <AlexisDemographics.hxx>

#include <GeneralState.hxx>
#include <Logger.hxx>
#include <RasterLoader.hxx>
#include <Statistics.hxx>

#include <limits>

namespace Gujarat
{

GujaratWorld::GujaratWorld( Engine::Simulation & simulation, const GujaratConfig & config ) 
	: World(simulation, config._homeRange+1, true, config._resultsFile), _agentKey(0), _climate(config,*this), _config(config)					
{
	// overlap is maxHomeRange + 1 to allow splits to be in adjacent worlds
	// TODO code a function proces config for resources 
	_yearlyBiomass.resize(4);
	_dailyRainSeasonBiomassIncrease.resize(4);
	_dailyDrySeasonBiomassDecrease.resize(4);

}

GujaratWorld::~GujaratWorld()
{
}    

/*
Engine::Point2D<int> GujaratWorld::findNearestWater( const Engine::Point2D<int> & point )
{
	Engine::Point2D<int> nearestWater(-1,-1);
	float bestDist = std::numeric_limits<float>::max();
	Engine::Point2D<int> index;
	bool found = false;
	int nextDist = 1;
	while(!found)
	{
		for(index._x=point._x-nextDist; index._x<point._x+nextDist; index._x++)
		{
			for(index._y=point._y-nextDist; index._y<point._y+nextDist; index._y++)
			{
				if(_overlapBoundaries.isInside(index) && getValue("soils", index)==WATER)
				{
					found = true;
					float dist = index.distance(point);
					if(dist<bestDist)
					{
						bestDist = dist;
						nearestWater = index;
					}
				}

			}
		}		
		nextDist++;
	}
	//std::cout << "final dist: " << nextDist << std::endl;
	return nearestWater;
}
*/

void GujaratWorld::createRasters()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " creating static rasters");
	registerStaticRaster("soils", _config.isStorageRequired("soils"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("soils"), _config._soilFile, this);	

	registerStaticRaster("dem", _config.isStorageRequired("dem"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("dem"), _config._demFile, this);

	registerStaticRaster("distWater", _config.isStorageRequired("distWater"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("distWater"), _config._distWaterFile, this);

	if(_config._biomassDistribution.compare("linDecayFromWater")==0 || _config._biomassDistribution.compare("logDecayFromWater")==0)
	{
		registerStaticRaster("weightWater", _config.isStorageRequired("weightWater"));
		Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("weightWater"), _config._weightWaterFile, this);
	}

	//registerDynamicRaster("weightWater", true);
	//getDynamicRaster("weightWater").setInitValues(0, std::numeric_limits<int>::max(), 0);
	/*
	registerStaticRaster("duneMap", _config.isStorageRequired("duneMap"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("duneMap"), _config._duneMapFile, this);
	*/

	log_DEBUG(logName.str(), getWallTime() << " creating dynamic rasters");
	/*
	registerDynamicRaster("moisture", _config.isStorageRequired("moisture"));
	getDynamicRaster("moisture").setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/
	registerDynamicRaster("resources", _config.isStorageRequired("resources"));
	getDynamicRaster("resources").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	//registerDynamicRaster("distWater", true);
	//getDynamicRaster("distWater").setInitValues(0, std::numeric_limits<int>::max(), 0);

	//std::stringstream logWater;
	//logWater<< "distwater_" << getId();
	//log_DEBUG( logName.str(), "timestep=" << getWorld()->getCurrentTimeStep());

	/*
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		std::cout << "row: " << index._x << std::endl;
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
		{
			int value = getValue("distWater", index);
			if(value==0)
			{
				setValue("weightWater", index, 0);
			}
			else
			{
				float weight = 8061.0f/(float)value;
				//std::cout << "weight with dist: " << value << " is: " << weight << std::endl;
				setValue("weightWater", index, weight*100.0f);
			}
		}
	}
	*/


	// we need to keep track of resource fractions
	registerDynamicRaster("resourcesFraction", false);
	getDynamicRaster("resourcesFraction").setInitValues(0, 100, 0);

	/*
	registerDynamicRaster("forageActivity", _config.isStorageRequired("forageActivity")); 
	getDynamicRaster("forageActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("homeActivity", _config.isStorageRequired("homeActivity"));
	getDynamicRaster("homeActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("farmingActivity", _config.isStorageRequired("farmingActivity"));
	getDynamicRaster("farmingActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/

	/*
	registerDynamicRaster("resourceType", _config.isStorageRequired("resourceType")); // type of resources: wild, domesticated or fallow
	getDynamicRaster("resourceType").setInitValues(0, SEASONALFALLOW, WILD);
	registerDynamicRaster("consecutiveYears", _config.isStorageRequired("consecutiveYears")); // years passed using a given cell for a particular use
	getDynamicRaster("consecutiveYears").setInitValues(0, 3, 0);
	registerDynamicRaster("sectors", _config.isStorageRequired("sectors"));
	getDynamicRaster("sectors").setInitValues(0, _config._numSectors, 0);
	*/

	/*
	registerDynamicRaster("tmpDunes", true); 
	getDynamicRaster("tmpDunes").setInitValues(0, 100, 0);
	*/

	log_DEBUG(logName.str(), getWallTime() << " generating settlement areas");
	_settlementAreas.generateAreas( *this, _config._lowResolution);
	//log_DEBUG(logName.str(), getWallTime() << " updating moisture");
	//updateMoisture();
	log_DEBUG(logName.str(), getWallTime() << " create rasters done");
}

void GujaratWorld::createAgents()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " creating agents");
	for(int i=0; i<_config._numHG; i++)
	{ 
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{			
			log_DEBUG(logName.str(), getWallTime() << " new HG with index: " << i);
			std::ostringstream oss;
 			oss << "HunterGatherer_" << i;
			HunterGatherer * agent = new HunterGatherer(oss.str());
			addAgent(agent);
			//_config._hgInitializer->initialize(agent);
			
			agent->createInitialPopulation(_config._adulthoodAge);

			agent->setSocialRange( _config._socialRange );
			agent->setHomeMobilityRange( _config._homeRange );
			agent->setHomeRange( _config._homeRange );
			//agent->setSurplusForReproductionThreshold( _config._surplusForReproductionThreshold );
			//agent->setSurplusWanted( _config._surplusWanted );
			//agent->setSurplusSpoilageFactor( _config._surplusSpoilage );
			
			//agent->setFoodNeedsForReproduction(_config._hgFoodNeedsForReproduction);			
			agent->setWalkingSpeedHour( _config._walkingSpeedHour / _config._cellResolution );
			agent->setForageTimeCost( _config._forageTimeCost );
			//agent->setAvailableForageTime( _config._availableForageTime );
			agent->setMassToCaloriesRate( _config._massToEnergyRate * _config._energyToCalRate );
			agent->setNumSectors( _config._numSectors );

			agent->initializePosition();
			log_DEBUG(logName.str(), getWallTime() << " new HG: " << agent);
		}
	}

	for(int i=0; i<_config._numAP; i++)
	{ 
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
 			oss << "AgroPastoralist_" << i;
			AgroPastoralist * agent = new AgroPastoralist(oss.str());
			addAgent(agent); 
			_config._apInitializer->initialize(agent);
			agent->setSocialRange( _config._socialRange );
			//agent->setSurplusSpoilageFactor( _config._surplusSpoilage );
			agent->setHomeMobilityRange( _config._socialRange );
			agent->setMaxCropHomeDistance( _config._maxCropHomeDistance );
			agent->setMassToCaloriesRate( _config._massToEnergyRate * _config._energyToCalRate );

			agent->initializePosition();
			std::cout << _simulation.getId() << " new AgroPastoralist: " << agent << std::endl;
		}
	}
}

float GujaratWorld::moistureFunction( const Soils & soilType, const float & rain, const Seasons & season )
{
//proposal: posar un objecte soil a la cel.la
//proposal: una subclass per cada tipus de soil i cadascuna implementa el comportament diferent: evaporacio, absorcio aigua,...
//proposal: put moisture, soilType inside Soil object (one for each cell)    
/* PENDENT
    proposal: 
    define a class array constant containing the alphas called moistureSoilAlpha[][]
    float m[3][3]={{0.0,.25,.5},{.0,.5,1.0},{.0,.75,1.5}};
    retrieve alpha with moistureSoilAlpha[soilType][season]
    moistureFunction( sT, r, s, mR) = r - meanRain*moistureSoilAlpha[soilType][season];
*/
	float newMoisture; // newMoisture = r - alpha * R;
	float alpha = 0.0;
	switch (soilType)
	{
		case WATER:
			switch (season)
			{
				case HOTWET:
					alpha = 0.0; // dropable case, initialization could cover it.
					break;
				case COLDDRY:
					alpha = 0.25; 
					break;
				case HOTDRY:
					alpha = 0.5;
			}	
			break;

		case INTERDUNE:
			switch (season)
			{
				case HOTWET:
					alpha = 0.0; // dropable case, initialization could cover it.
					break;
				case COLDDRY:
					alpha = 0.5;
					break;
				case HOTDRY:
					alpha = 1.0;
			}
			break;
		
		case DUNE:
			switch (season)
			{
				case HOTWET:
					alpha = 0.0; // dropable case, initialization could cover it.
					break;
				case COLDDRY:
					alpha = 0.75;
					break;
				case HOTDRY:
					alpha = 1.5;
			}
	}
	//std::cout << "step: " << _step << " alpha: " << alpha << " with season: " << season << " and soil type: " << soilType << std::endl;
	newMoisture = std::max(0.0f, rain - (alpha * _climate.getMeanAnnualRain()));
	return newMoisture;
}

/*------------------------------*/

void GujaratWorld::updateRainfall()
{		
	_climate.step();
}

/*
void GujaratWorld::updateMoisture()
{
	float rain = _climate.getRain();
	Seasons season = _climate.getSeason();

	//std::cout << "in step: " << _step << " season: " << season << std::endl;
	//foreach c in cells
	//	c.moisture = moistureFunction( c.soilType, rain, s, m);

	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)			
		{
			//int value    = getValue(key, index);
			//int maxValue = getMaxValue(key, index);
			Soils soilT = (Soils)getValue("soils", index);
			float moisture = moistureFunction( soilT, rain, season );
			setValue("moisture", index, moisture);
		}
	}
}
*/

void GujaratWorld::updateSoilCondition()
{
	Engine::Point2D<int> index;
	if(_climate.getSeason()==HOTWET)
	{
		for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
		{
			for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
			{
				setValue("resources", index, getValue("moisture", index));
				if(getValue("resourceType", index)==WILD)
				{
					continue;
				}
				
				if(getValue("resourceType", index)==SEASONALFALLOW)
				{
					setValue("resourceType", index, DOMESTICATED);
				}
				int consecutiveYears = getValue("consecutiveYears", index);
				consecutiveYears++;				
				if(consecutiveYears<3)
				{
					setValue("consecutiveYears", index, consecutiveYears);
				}
				else
				{
					setValue("consecutiveYears", index, 0);
					if(getValue("resourceType", index)==FALLOW)
					{
						setValue("resourceType", index, WILD);

					}
					else
					{
						setValue("resourceType", index, FALLOW);
					}
				}
			}
		}
	}
	else if(_climate.getSeason()==HOTDRY)
	{
		for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
		{
			for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)
			{
				if(getValue("resourceType", index)==DOMESTICATED)
				{
					setValue("resourceType", index, SEASONALFALLOW);
				}
			}
		}
	}
}

void GujaratWorld::updateResources()
{
	Engine::Point2D<int> index;
	for( index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++ )		
	{
		for( index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++ )
		{
			// 3. Increment or Decrement cell biomass depending on yearly biomass
			//    figures and current timestep
			int currentValue = getValue("resources", index);
			float currentFraction = (float)getValue("resourcesFraction", index)/100.0f;
			Soils cellSoil = (Soils)getValue("soils", index);
			if(cellSoil!=WATER)
			{
				Seasons season = _climate.getSeason();
				bool wetSeason = false;
				if(season==HOTWET)
				{
					wetSeason = true;
				}			
				float newValue = std::max(0.0f, currentValue+currentFraction+getBiomassVariation(wetSeason, cellSoil, index));
				currentValue = newValue;
				float fraction = 100.0f*(newValue  - currentValue);
				setValue("resources", index, currentValue);
				setValue("resourcesFraction", index, (int)fraction);
			}
		}
	}
}

void GujaratWorld::recomputeYearlyBiomass()
{
	std::stringstream logName;
	logName << "World_" << _simulation.getId();
	// 1. Compute factor between actual rain and average rain		
	float raininessFactor = _climate.getRain() / _climate.getMeanAnnualRain();
	
	// each cell is 31.5m * 31.5m
	double areaOfCell = _config._cellResolution * _config._cellResolution;

	// 2. For each soil type compute yearly biomass	

	// data expressed in g/m2
	_yearlyBiomass[WATER] = 0.0f;
	_yearlyBiomass[DUNE] = areaOfCell*_config._duneBiomass * raininessFactor * _config._duneEfficiency;
	_yearlyBiomass[INTERDUNE] = areaOfCell*_config._interduneBiomass * _config._interduneEfficiency * raininessFactor;
	
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " rain: " << _climate.getRain() << " with mean: " << _climate.getMeanAnnualRain());
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " yearly biomass of dune: " << _yearlyBiomass[DUNE] << " and interdune: " << _yearlyBiomass[INTERDUNE]);

	// yearly biomass is the area of a triangle with max height at the end of wet season
	// A_1 + A_2 = biomass, being A_1 = daysPerSeason*h/2 and A_2 = 2*daysPerSeason*h/2
	// dPS*h/2 + 2*dPS*h/2 = biomass, so h = biomass/1.5*dPS
	// and A_2 = 2*A_1

	double heightInterDune = _yearlyBiomass[INTERDUNE]/(_config._daysPerSeason*3/2);
	_dailyRainSeasonBiomassIncrease[INTERDUNE] = heightInterDune/_config._daysPerSeason;
	_dailyDrySeasonBiomassDecrease[INTERDUNE] = heightInterDune/(2*_config._daysPerSeason);
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " biomass height interdune: " << heightInterDune << " increment: " << _dailyRainSeasonBiomassIncrease[INTERDUNE] << " and decrease: " << _dailyDrySeasonBiomassDecrease[INTERDUNE]);
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " calories height interdune: " << _config._massToEnergyRate*_config._energyToCalRate*heightInterDune << " increment: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyRainSeasonBiomassIncrease[INTERDUNE] << " and decrease: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyDrySeasonBiomassDecrease[INTERDUNE]);

	double heightDune = _yearlyBiomass[DUNE]/(_config._daysPerSeason*3/2);
	_dailyRainSeasonBiomassIncrease[DUNE] = heightDune/_config._daysPerSeason;
	_dailyDrySeasonBiomassDecrease[DUNE] = heightDune/(2*_config._daysPerSeason);
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " biomass height dune: " << heightDune << " increment: " << _dailyRainSeasonBiomassIncrease[DUNE] << " and decrease: " << _dailyDrySeasonBiomassDecrease[DUNE]);
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " calories height dune: " << _config._massToEnergyRate*_config._energyToCalRate*heightDune << " increment: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyRainSeasonBiomassIncrease[DUNE] << " and decrease: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyDrySeasonBiomassDecrease[DUNE]);

	_dailyRainSeasonBiomassIncrease[WATER] = 0.0f;
	_dailyDrySeasonBiomassDecrease[WATER] = 0.0f;
	
	/*
	// TODO remove
	Engine::Point2D<int> index;
	for( index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++ )		
	{
		for( index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++ )
		{
			Soils cellSoil = (Soils)getValue("soils", index);
			if(cellSoil==INTERDUNE)
			{
				setValue("resources", index, heightInterDune);
			}
			else if(cellSoil==DUNE)
			{
				setValue("resources", index, heightDune);
			}
		}
	}
	*/
}

void GujaratWorld::stepEnvironment()
{
	// at the end of simulation
	_climate.advanceSeason();

	std::stringstream logName;
	logName << "World_" << _simulation.getId();

	// if this is the first step of a wet season, rainfall and biomass are calculated for the entire year
	if ( _climate.rainSeasonStarted() )
	{
		log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " num agents: " << _agents.size());
		updateRainfall();
		recomputeYearlyBiomass();
	}
	// resources are updated each time step
	updateResources();
	getDynamicRaster("resources").updateCurrentMinMaxValues();

	unsigned nrAdults = 0;
	for ( AgentsList::iterator it = _agents.begin(); 
		it != _agents.end(); it++ )
		nrAdults += dynamic_cast<GujaratAgent*>((*it))->getNrAvailableAdults();	

	unsigned nrChildren = 0;
	for ( AgentsList::iterator it = _agents.begin(); 
		it != _agents.end(); it++ )
		nrChildren += dynamic_cast<GujaratAgent*>((*it))->getNrChildren();

	// these rasters are only updated at the beginning of seasons
//	if ( !_climate.cellUpdateRequired() ) return;


	//updateMoisture();
	//updateSoilCondition();
}

const Climate & GujaratWorld::getClimate() const
{
	return _climate;
}

long int GujaratWorld::getNewKey()
{
	return _agentKey++;
}

bool	GujaratWorld::isInterdune( Engine::Point2D<int> p )
{
	return getValue("soils", p) == INTERDUNE;
}

bool	GujaratWorld::isWild( Engine::Point2D<int> p )
{
	return getValue( "resourceType", p ) == WILD;
}

bool	GujaratWorld::isColdDrySeason()
{
	return getClimate().getSeason() == COLDDRY;
}
	
float GujaratWorld::getBiomassVariation( bool wetSeason, Soils & cellSoil, const Engine::Point2D<int> & index ) const
{
	double variation = 0.0f;
	if(_config._biomassDistribution.compare("standard")==0)
	{
		if(wetSeason)
		{
			variation = _dailyRainSeasonBiomassIncrease.at(cellSoil);
		}
		else
		{
			variation = -_dailyDrySeasonBiomassDecrease.at(cellSoil);
		}
	}
	else if(_config._biomassDistribution.compare("linDecayFromWater")==0 || _config._biomassDistribution.compare("logDecayFromWater")==0)
	{
		variation = _config._waterDistConstant*getValue("weightWater", index);
		//variation = 1600.0f*1600.0f*float(getValue("weightWater", index))/16423239174.0f;
		if(wetSeason)
		{
			variation *= (float)_dailyRainSeasonBiomassIncrease.at(cellSoil);			
		}
		else
		{
			variation *= -1.0f*(float)_dailyDrySeasonBiomassDecrease.at(cellSoil);
		}
		//std::cout << "variation: " << variation << " constant: " << _config._waterDistConstant << " weight: " << getValue("weightWater", index) << " increase: " <<_dailyRainSeasonBiomassIncrease.at(cellSoil) << " dist to water: " << getValue("distWater", index) << std::endl;
		//variation /= 100.0f;
	}
	else
	{
		std::stringstream oss;
		oss << "GujaratWorld::getBiomassVariation - unknown biomass distribution: " << _config._biomassDistribution;
		throw Engine::Exception(oss.str());

	}
	return variation;
}

} // namespace Gujarat

