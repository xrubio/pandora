
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

void GujaratWorld::createRasters()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " creating static rasters");
	registerStaticRaster("soils", _config.isStorageRequired("soils"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("soils"), _config._soilFile, this);	

	registerStaticRaster("dem", _config.isStorageRequired("dem"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("dem"), _config._demFile, this);

	registerStaticRaster("duneMap", _config.isStorageRequired("duneMap"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("duneMap"), _config._duneMapFile, this);

	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " creating dynamic rasters");
	/*
	registerDynamicRaster("moisture", _config.isStorageRequired("moisture"));
	getDynamicRaster("moisture").setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/
	registerDynamicRaster("resources", _config.isStorageRequired("resources"));
	getDynamicRaster("resources").setInitValues(0, std::numeric_limits<int>::max(), 0);
	// we need to keep track of resource fractions
	registerDynamicRaster("resourcesFraction", false);
	getDynamicRaster("resourcesFraction").setInitValues(0, 100, 0);
	
	registerDynamicRaster("forageActivity", _config.isStorageRequired("forageActivity")); 
	getDynamicRaster("forageActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("homeActivity", _config.isStorageRequired("homeActivity"));
	getDynamicRaster("homeActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("farmingActivity", _config.isStorageRequired("farmingActivity"));
	getDynamicRaster("farmingActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("resourceType", _config.isStorageRequired("resourceType")); // type of resources: wild, domesticated or fallow
	getDynamicRaster("resourceType").setInitValues(0, SEASONALFALLOW, WILD);
	registerDynamicRaster("consecutiveYears", _config.isStorageRequired("consecutiveYears")); // years passed using a given cell for a particular use
	getDynamicRaster("consecutiveYears").setInitValues(0, 3, 0);
	registerDynamicRaster("sectors", _config.isStorageRequired("sectors"));
	getDynamicRaster("sectors").setInitValues(0, _config._numSectors, 0);

	/*
	registerDynamicRaster("tmpDunes", true); 
	getDynamicRaster("tmpDunes").setInitValues(0, 100, 0);
	*/

	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " generating settlement areas");
	_settlementAreas.generateAreas( *this, _config._lowResolution);
	//log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " updating moisture");
	//updateMoisture();
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " create rasters done");
}

void GujaratWorld::createAgents()
{
	std::stringstream logName;
	logName << "simulation_" << _simulation.getId();
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " creating agents");
	for(int i=0; i<_config._numHG; i++)
	{ 
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{			
			log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " new HG with index: " << i);
			std::ostringstream oss;
 			oss << "HunterGatherer_" << i;
			HunterGatherer * agent = new HunterGatherer(oss.str());
			addAgent(agent);
			//_config._hgInitializer->initialize(agent);
			
			agent->createInitialPopulation();

			agent->setSocialRange( _config._socialRange );
			agent->setHomeMobilityRange( _config._homeRange );
			agent->setHomeRange( _config._homeRange );
			//agent->setSurplusForReproductionThreshold( _config._surplusForReproductionThreshold );
			//agent->setSurplusWanted( _config._surplusWanted );
			//agent->setSurplusSpoilageFactor( _config._surplusSpoilage );
			
			//agent->setFoodNeedsForReproduction(_config._hgFoodNeedsForReproduction);			
			agent->setWalkingSpeedHour( _config._walkingSpeedHour / _config._cellResolution );
			agent->setForageTimeCost( _config._forageTimeCost );
			agent->setAvailableForageTime( _config._availableForageTime );
			agent->setMassToCaloriesRate( _config._massToEnergyRate * _config._energyToCalRate );
			agent->setNumSectors( _config._numSectors );

			agent->initializePosition();
			log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " new HG: " << agent);
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
			/*
			int value    = getValue(key, index);
			int maxValue = getMaxValue(key, index);
			*/
			Soils soilT = (Soils)getValue("soils", index);
			float moisture = moistureFunction( soilT, rain, season );
			setValue("moisture", index, moisture);
		}
	}
}

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
				// increasing biomass
				float newValue = 0;
				if(season==HOTWET)
				{
					newValue = std::max(0.0f, currentValue+currentFraction+_dailyRainSeasonBiomassIncrease[cellSoil]);
					//setValue("resources", index,  std::max(0.0f, currentValue+_dailyRainSeasonBiomassIncrease[cellSoil]));
				}			
				else
				{
					newValue = std::max(0.0f, currentValue+currentFraction-_dailyDrySeasonBiomassDecrease[cellSoil]);
				}
				currentValue = newValue;
				float fraction = 100.0f*(newValue  - currentValue);
				//std::cout << "new value: " << newValue << " int: " << currentValue << " fraction: " << (int)fraction << std::endl;
				setValue("resources", index, currentValue);
				setValue("resourcesFraction", index, (int)fraction);
			}
		}
	}
}

void GujaratWorld::recomputeYearlyBiomass()
{
	// 1. Compute factor between actual rain and average rain		
	float raininessFactor = _climate.getRain() / _climate.getMeanAnnualRain();
	
	// each cell is 31.5m * 31.5m
	double areaOfCell = _config._cellResolution * _config._cellResolution;

	// 2. For each soil type compute yearly biomass	

	// data expressed in g/m2
	_yearlyBiomass[WATER] = 0.0f;
	_yearlyBiomass[DUNE] = areaOfCell*_config._duneBiomass * raininessFactor * _config._duneEfficiency;
	_yearlyBiomass[INTERDUNE] = areaOfCell*_config._interduneBiomass * _config._interduneEfficiency * raininessFactor;
	std::cout << "step: " << getCurrentStep() << " yearly biomass of dune: " << _yearlyBiomass[DUNE] << " and interdune: " << _yearlyBiomass[INTERDUNE] << " with rain: " << _climate.getRain() << " and mean rain: " << _climate.getMeanAnnualRain() << std::endl;


	// yearly biomass is the area of a triangle with max height at the end of wet season
	// A_1 + A_2 = biomass, being A_1 = daysPerSeason*h/2 and A_2 = 2*daysPerSeason*h/2
	// dPS*h/2 + 2*dPS*h/2 = biomass, so h = biomass/1.5*dPS
	// and A_2 = 2*A_1

	double heightInterDune = _yearlyBiomass[INTERDUNE]/(_config._daysPerSeason*3/2);
	_dailyRainSeasonBiomassIncrease[INTERDUNE] = heightInterDune/_config._daysPerSeason;
	_dailyDrySeasonBiomassDecrease[INTERDUNE] = heightInterDune/(2*_config._daysPerSeason);
	std::cout << "height interdune: " << heightInterDune << " increment: " << _dailyRainSeasonBiomassIncrease[INTERDUNE] << " and decrease: " << _dailyDrySeasonBiomassDecrease[INTERDUNE] << std::endl;

	double heightDune = _yearlyBiomass[DUNE]/(_config._daysPerSeason*3/2);
	_dailyRainSeasonBiomassIncrease[DUNE] = heightDune/_config._daysPerSeason;
	_dailyDrySeasonBiomassDecrease[DUNE] = heightDune/(2*_config._daysPerSeason);
	std::cout << "height dune: " << heightDune << " increment: " << _dailyRainSeasonBiomassIncrease[DUNE] << " and decrease: " << _dailyDrySeasonBiomassDecrease[DUNE] << std::endl;

	_dailyRainSeasonBiomassIncrease[WATER] = 0.0f;
	_dailyDrySeasonBiomassDecrease[WATER] = 0.0f;
}

void GujaratWorld::stepEnvironment()
{
	// at the end of simulation
	_climate.advanceSeason();
	// if this is the first step of a wet season, rainfall and biomass are calculated for the entire year
	if ( _climate.rainSeasonStarted() )
	{
		updateRainfall();
		recomputeYearlyBiomass();
	}
	// resources are updated each time step
	updateResources();
	getDynamicRaster("resources").updateCurrentMinMaxValues();

	std::stringstream logName;
	logName << "World_" << _simulation.getId();

	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << " timestep=" << getCurrentTimeStep());
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << "\tagentPopulation=" << _agents.size());

	unsigned nrAdults = 0;
	for ( AgentsList::iterator it = _agents.begin(); 
		it != _agents.end(); it++ )
		nrAdults += dynamic_cast<GujaratAgent*>((*it))->getNrAvailableAdults();	

	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << "\tadultPopulation=" << nrAdults );

	unsigned nrChildren = 0;
	for ( AgentsList::iterator it = _agents.begin(); 
		it != _agents.end(); it++ )
		nrChildren += dynamic_cast<GujaratAgent*>((*it))->getNrChildren();

	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << "\tchildrenPopulation=" << nrChildren);
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << "\tmaxCurrentResources=" << getDynamicRaster("resources").getCurrentMaxValue());
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << "\tminCurrentResources=" << getDynamicRaster("resources").getCurrentMinValue());
	log_DEBUG(logName.str(), MPI_Wtime() - _initialTime << "\tavgCurrentResources=" << getDynamicRaster("resources").getAvgValue() );

	// these rasters are only updated at the beginning of seasons
	if ( !_climate.cellUpdateRequired() ) return;


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

} // namespace Gujarat

