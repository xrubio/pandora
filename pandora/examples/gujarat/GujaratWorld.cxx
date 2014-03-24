#include <stdlib.h>  

#include <GujaratWorld.hxx>
#include <GujaratState.hxx>

#include <Raster.hxx>
#include <Point2D.hxx>
#include <HunterGatherer.hxx>

//#include <AgroPastoralist.hxx>
#include <Exceptions.hxx>
#include <GujaratConfig.hxx>
#include <OriginalDemographics.hxx>

#include <GeneralState.hxx>

#include <Logger.hxx>
#include <RasterLoader.hxx>
#include <Statistics.hxx>
#include <iomanip>
#include <algorithm>

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
	_remainingBiomass.resize(4);
	_remainingBiomass[3] = _remainingBiomass[2] = _remainingBiomass[1] = 0.0f;
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
	
	registerStaticRaster("soils", _config.isStorageRequired("soils"), eSoils);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eSoils), _config._soilFile, this);	
			
	registerStaticRaster("dem", _config.isStorageRequired("dem"), eDem);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eDem), _config._demFile, this);

	registerStaticRaster("distWater", _config.isStorageRequired("distWater"), eDistWater);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eDistWater), _config._distWaterFile, this);

	if(_config._biomassDistribution.compare("linDecayFromWater")==0 || _config._biomassDistribution.compare("logDecayFromWater")==0)
	{
		registerStaticRaster("weightWater", _config.isStorageRequired("weightWater"), eWeightWater);
		Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eWeightWater), _config._weightWaterFile, this);
	}

	//registerDynamicRaster("weightWater", true);
	//getDynamicRaster("weightWater").setInitValues(0, std::numeric_limits<int>::max(), 0);
	/*
	registerStaticRaster("duneMap", eDuneMap, _config.isStorageRequired("duneMap"));
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("duneMap"), _config._duneMapFile, this);
	*/

	log_DEBUG(logName.str(), getWallTime() << " creating dynamic rasters");

	/*
	registerDynamicRaster("moisture", _config.isStorageRequired("moisture"));
	getDynamicRaster("moisture").setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/
	
	registerDynamicRaster("resources", _config.isStorageRequired("resources"), eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);	
	
	// we need to keep track of resource fractions
	registerDynamicRaster("resourcesFraction", false, eResourcesFraction);
	getDynamicRaster(eResourcesFraction).setInitValues(0, 100, 0);

	registerDynamicRaster("forageActivity", _config.isStorageRequired("forageActivity"), eForageActivity); 
	getDynamicRaster(eForageActivity).setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	
	/*
	registerDynamicRaster("homeActivity", eHomeActivity, _config.isStorageRequired("homeActivity"));
	getDynamicRaster("homeActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	registerDynamicRaster("farmingActivity", eFarmingActivity, _config.isStorageRequired("farmingActivity"));
	getDynamicRaster("farmingActivity").setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/

	/*
	registerDynamicRaster("resourceType", eResourceType, _config.isStorageRequired("resourceType")); // type of resources: wild, domesticated or fallow
	getDynamicRaster("resourceType").setInitValues(0, SEASONALFALLOW, WILD);
	registerDynamicRaster("consecutiveYears", eConsecutiveYears, _config.isStorageRequired("consecutiveYears")); // years passed using a given cell for a particular use
	getDynamicRaster("consecutiveYears").setInitValues(0, 3, 0);
	registerDynamicRaster("sectors", eSectors, _config.isStorageRequired("sectors"));
	getDynamicRaster("sectors").setInitValues(0, _config._numSectors, 0);
	*/

	log_DEBUG(logName.str(), getWallTime() << " generating settlement areas");
	_settlementAreas.generateAreas( *this, _config._lowResolution);
	log_DEBUG(logName.str(), getWallTime() << " create rasters done");
	
	assert(_settlementAreas.getAreas().size() > 0);

	registerDynamicRaster("counterHRSectors", _config.isStorageRequired("counterHRSectors"), eCounterHRSectors); 
	getDynamicRaster(eCounterHRSectors).setInitValues(-1, std::numeric_limits<int>::max(), -1);
	fillIniRaster(eCounterHRSectors,-1);

	registerDynamicRaster("counterLRSectors", _config.isStorageRequired("counterLRSectors"), eCounterLRSectors); 
	getDynamicRaster(eCounterLRSectors).setInitValues(-1, std::numeric_limits<int>::max(), -1);
	fillIniRaster(eCounterLRSectors,-1);
	
	
	
std::cout << "init LR" << std::endl;	
	
	// Low Ressolution Rasters
	int LowResRasterSideSize = getLowResMapsSideSize();
	Engine::Point2D<int> lowResSize2D( LowResRasterSideSize, LowResRasterSideSize);
	
	registerDynamicRaster("eLRResources", _config.isStorageRequired("eLRResources")
			, eLRResources, lowResSize2D);
	getDynamicRaster(eLRResources).setInitValues(0, std::numeric_limits<int>::max(), 0);
	fillLRRaster(eLRResources, 0);
	
	registerDynamicRaster("eLRResourcesFraction", _config.isStorageRequired("eLRResourcesFraction")
			, eLRResourcesFraction, lowResSize2D);
	getDynamicRaster(eLRResourcesFraction).setInitValues(0, std::numeric_limits<int>::max(), 0);
	fillLRRaster(eLRResourcesFraction, 0);
	
	registerDynamicRaster("eLRForageActivity", _config.isStorageRequired("eLRForageActivity")
			, eLRForageActivity, lowResSize2D);
	getDynamicRaster(eLRForageActivity).setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("eLRHomeActivity", _config.isStorageRequired("eLRHomeActivity")
			, eLRHomeActivity, lowResSize2D);
	getDynamicRaster(eLRHomeActivity).setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("eLRFarmingActivity", _config.isStorageRequired("eLRFarmingActivity")
			, eLRFarmingActivity, lowResSize2D);
	getDynamicRaster(eLRFarmingActivity).setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("eLRPopulation", _config.isStorageRequired("eLRPopulation")
			, eLRPopulation, lowResSize2D);
	getDynamicRaster(eLRPopulation).setInitValues(0, std::numeric_limits<int>::max(), 0);
	fillLRRaster(eLRPopulation, 0);
	
	
	/*registerDynamicRaster("eLRMoisture", _config.isStorageRequired("eLRMoisture")
			, eLRMoisture, Engine::Point2D<int>(LowResRasterSideSize,LowResRasterSideSize));
	getDynamicRaster(eLRMoisture).setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/
	// Low Ressolution Soil Counters
	
std::cout << "init LR dune" << std::endl;		

std::cout << "max "<< _config._lowResolution*_config._lowResolution << std::endl;		

	registerStaticRaster("LRCounterSoilDUNE", false, LRCounterSoilDUNE, lowResSize2D );
	getStaticRaster(LRCounterSoilDUNE).setDefaultInitValues(0, _config._lowResolution*_config._lowResolution, 0);	
	fillLowResCounterRaster(LRCounterSoilDUNE,eSoils,DUNE);
	
std::cout << "init LR interdune" << std::endl;		
	
	registerStaticRaster("LRCounterSoilINTERDUNE", false, LRCounterSoilINTERDUNE, lowResSize2D);
	getStaticRaster(LRCounterSoilINTERDUNE).setDefaultInitValues(0, _config._lowResolution*_config._lowResolution, 0);
	fillLowResCounterRaster(LRCounterSoilINTERDUNE,eSoils,INTERDUNE);
	
std::cout << "init LR water" << std::endl;		
	

	if(_config._biomassDistribution.compare("linDecayFromWater")==0 || _config._biomassDistribution.compare("logDecayFromWater")==0)
	{
	
		registerStaticRaster("LRCounterSoilWATER", false, LRCounterSoilWATER, lowResSize2D);
		getStaticRaster(LRCounterSoilWATER).setDefaultInitValues(0, _config._lowResolution*_config._lowResolution, 0);
		fillLowResCounterRaster(LRCounterSoilWATER,eSoils,WATER);

		std::cout << "init LR weightwater" << std::endl;		
	
		registerStaticRaster("eLRWeightWater", false, eLRWeightWater, lowResSize2D);
		getStaticRaster(eLRWeightWater).setDefaultInitValues(0, std::numeric_limits<int>::max(), 0);
		fillLowResMeanRaster(eLRWeightWater,eWeightWater);
	}
std::cout << "end init LR" << std::endl;	

 
}


void GujaratWorld::fillLRRaster(enum Rasters idLRRaster, int val)
{
	Engine::Point2D<int> index;
	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{			
			Engine::Point2D<int> mapCell; 
			worldCell2LowResCell(index, mapCell);
			setInitValueLR(idLRRaster,mapCell,val);
			//getDynamicRaster(idLRRaster).setValue(mapCell,val);
			//setValueLR(idLRRaster,mapCell,val);
		}
	}
}


void GujaratWorld::fillHRRasterWithLRRaster(enum Rasters idLRSource, enum Rasters idHRTarget)
{
	Engine::Point2D<int> index;
	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{			
			Engine::Point2D<int> mapCell; 
			worldCell2LowResCell(index, mapCell);
			getDynamicRaster(idHRTarget).setValue(index, getValueLR(idLRSource,mapCell));
		}
	}
}


void GujaratWorld::fillIniRaster(enum Rasters idRaster, int val)
{
	Engine::Point2D<int> index;
	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{		
			//*?
			//getDynamicRaster(idHRTarget).setValue(index, val);
			setValue(idRaster, index, val);
		}
	}
	
}



void GujaratWorld::fillRaster(enum Rasters idRaster, int val)
{
	Engine::Point2D<int> index;
	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{			
			//*? 
			//getDynamicRaster(idRaster).setValue(index, val);
			setValue(idRaster, index, val);
			//setInitValue(idRaster, index, val);
		}
	}
	
}


void GujaratWorld::fillLowResCounterRaster(enum Rasters idRasterCounter, enum Rasters idRasterSource,int target)
{
	Engine::Point2D<int> index;
	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{
			int val = getValue(idRasterSource,index);
			Engine::Point2D<int> mapCell; 
			worldCell2LowResCell(index, mapCell);
			int count = getValueLR(idRasterCounter,mapCell);
			if(val==target)
			{
				setInitValueLR(idRasterCounter,mapCell,count+1);
				//getDynamicRaster(idRasterCounter).setValue(mapCell,count+1);
				//setValueLR(idRasterCounter,mapCell,count+1);
			}
			
		}
	}
}

void GujaratWorld::LowResRasterCountsHighResRaster(enum Rasters idRasterCounter, enum Rasters idRasterSource)
{
	Engine::Point2D<int> index;
	
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{
			Engine::Point2D<int> mapCell; 
			worldCell2LowResCell(index, mapCell);
			int count = getValueLR(idRasterCounter,mapCell);
			int val   = getValue(idRasterSource,index);			
			//*?
			//setValueLR(idRasterCounter,mapCell,count+val);
			setInitValueLR(idRasterCounter,mapCell,count+val);
			//getDynamicRaster(idRasterCounter).setValue(mapCell,count+val);
		}
	}
}


void GujaratWorld::fillLowResMeanRaster(enum Rasters idRasterCounter, enum Rasters idRasterSource)
{
	Engine::Point2D<int> index;
	for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
	{	
		for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
		{
			int val = getValue(idRasterSource,index);
			Engine::Point2D<int> mapCell; 
			worldCell2LowResCell( index, mapCell);
			int count = getValueLR(idRasterCounter,mapCell);
			setInitValueLR(idRasterCounter,mapCell,count+val);
			//getDynamicRaster(idRasterCounter).setValue(mapCell,count+val);
			//setValueLR(idRasterCounter,mapCell,count+val);
		}
	}
	
	for(	index._x=_boundaries._origin._x/_config._lowResolution; 
			index._x<(_boundaries._origin._x+_boundaries._size._x)/_config._lowResolution; 
			index._x++)		
	{	
		for( index._y=_boundaries._origin._y/_config._lowResolution; 
			 index._y<(_boundaries._origin._y+_boundaries._size._y)/_config._lowResolution; 
			 index._y++)		
		{				
//			std::cout << "get value at " << index << std::endl;
			int count = getValueLR(idRasterCounter,index);
			setInitValueLR(idRasterCounter
						,index						,count/(_config._lowResolution*_config._lowResolution));
			/*setValueLR(idRasterCounter
						,index						,count/(_config._lowResolution*_config._lowResolution));*/
			//getDynamicRaster(idRasterCounter).setValue(index,count/(_config._lowResolution*_config._lowResolution));
		}
	}
}

int GujaratWorld::getValueGW( const Engine::Raster & r, const Engine::Point2D<int> & position ) const
{
	Engine::Point2D<int> localPosition(position - _overlapBoundaries._origin);
	return r.getValue(localPosition);
}



int GujaratWorld::getValueLR( const int & index, const Engine::Point2D<int> & position ) const
{
	Engine::Point2D<int> originLR;
	worldCell2LowResCell( _overlapBoundaries._origin, originLR );
	Engine::Point2D<int> localPosition(position - originLR);
	return _rasters.at(index)->getValue(localPosition);
}


int GujaratWorld::getValueLR( const Engine::Raster & r, const Engine::Point2D<int> & position ) const
{
	Engine::Point2D<int> originLR;
	worldCell2LowResCell( _overlapBoundaries._origin, originLR );
	Engine::Point2D<int> localPosition(position - originLR);
	return r.getValue(localPosition);
}


void GujaratWorld::setValueLR( const int & index, const Engine::Point2D<int> & position, int value )
{
	
	Engine::Point2D<int> originLR;
	worldCell2LowResCell( _overlapBoundaries._origin, originLR );
	Engine::Point2D<int> localPosition(position - originLR);
	((Engine::Raster*)_rasters.at(index))->setValue(localPosition, value);
}

void GujaratWorld::setValueLR( Engine::Raster & r, const Engine::Point2D<int> & position, int value )
{
	Engine::Point2D<int> originLR;
	worldCell2LowResCell( _overlapBoundaries._origin, originLR );
	Engine::Point2D<int> localPosition(position - originLR);
	r.setValue(localPosition, value);
}

void GujaratWorld::setInitValueLR( const int & index, const Engine::Point2D<int> & position, int value )
{
	Engine::Point2D<int> originLR;
	worldCell2LowResCell( _overlapBoundaries._origin, originLR );
	Engine::Point2D<int> localPosition(position - originLR);
	((Engine::Raster*)_rasters.at(index))->setInitValue(localPosition, value);
}

void GujaratWorld::setInitValueLR( Engine::Raster & r, const Engine::Point2D<int> & position, int value )
{
	Engine::Point2D<int> originLR;
	worldCell2LowResCell( _overlapBoundaries._origin, originLR );
	Engine::Point2D<int> localPosition(position - originLR);
	r.setInitValue(localPosition, value);
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
			agent->createMind();
			
			
			agent->createInitialPopulation(_config._adulthoodAge);

			agent->setSocialRange( _config._socialRange );
			agent->setHomeMobilityRange( _config._homeRange );
			agent->setHomeRange( _config._homeRange );
			agent->setLowResHomeRange( _config._lowResHomeRange );
			//agent->setSurplusForReproductionThreshold( _config._surplusForReproductionThreshold );
			//agent->setSurplusWanted( _config._surplusWanted );
			//agent->setSurplusSpoilageFactor( _config._surplusSpoilage );

			//agent->setFoodNeedsForReproduction(_config._hgFoodNeedsForReproduction);			
			agent->setWalkingSpeedHour( _config._walkingSpeedHour / _config._cellResolution );
			agent->setForageTimeCost( _config._forageTimeCost );
			//agent->setAvailableForageTime( _config._availableForageTime );
			agent->setMassToCaloriesRate( _config._massToEnergyRate * _config._energyToCalRate );
			agent->setNumSectors( _config._numSectors );

			
			if ( GujaratState::getInitialPosition()._x < 0 || GujaratState::getInitialPosition()._y < 0 )
			{
				agent->initializePosition();
			}
			else
			{
				agent->initializePosition(GujaratState::getInitialPosition());			
			}
			

			log_DEBUG(logName.str(), getWallTime() << " new HG: " << agent);

			Engine::Point2D<int> LRpos;
			worldCell2LowResCell(agent->getPosition(),LRpos);
			setValueLR(eLRPopulation,LRpos,1+getValueLR(eLRPopulation,LRpos));

			std::stringstream logName2;
			logName2 << "agents_" << getId();

			log_INFO(logName2.str(), "created agent: " << agent << " available time: " << agent->getAvailableTime() << " needed resources: " << agent->computeConsumedResources(1));

		}
	}

	/*	
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
	*/


	
}


void GujaratWorld::getHRFreeCell(const Engine::Point2D<int> LRpos, Engine::Point2D<int> & HRpos)
{
	
	// posa un bucle recorrent la LR cell, comença a un punt random i recorres modularment la LRcell
	
	int C = getConfig()._lowResolution;
	
	Engine::Point2D<int> cornerLeftUp;
	Engine::Point2D<int> cornerRightDown;
	// 4 corners that bound the world cells belonging to the low res cell
	cornerLeftUp._x = LRpos._x*C;
	cornerLeftUp._y = LRpos._y*C;
	cornerRightDown._x = LRpos._x*C + C-1;
	cornerRightDown._y = LRpos._y*C + C-1;
	//*?
assert(cornerLeftUp._x<=cornerRightDown._x);
assert(cornerLeftUp._y<=cornerRightDown._y);
	HRpos._x = cornerLeftUp._x + rand()%(cornerRightDown._x - cornerLeftUp._x+1);
	HRpos._y = cornerLeftUp._y + rand()%(cornerRightDown._y - cornerLeftUp._y+1);
	//*?HRpos._x =  Engine::GeneralState::statistics().getUniformDistValue(cornerLeftUp._x,cornerRightDown._x);
	//*?HRpos._y = 
	//*?Engine::GeneralState::statistics().getUniformDistValue(cornerLeftUp._y,cornerRightDown._y);
	//TODO checkPosition expensive??? cost : #agents
	while( getValue(eSoils,HRpos) != DUNE || ! Engine::World::checkPosition(HRpos))
	{
		HRpos._y++;
		if(HRpos._y > cornerRightDown._y)
		{
			HRpos._y = cornerLeftUp._y;
			HRpos._x++;
			if( HRpos._x > cornerRightDown._x)
			{
				HRpos._x = cornerLeftUp._x;
			}
		}
	// Ensured by MoveHomeAction : there are DUNEs, and there are not too
	// much agents occupying cells. The loop will end finding a free cell.
	}
	// Ensured by MoveHomeAction : HRpos is a DUNE, and there are no agents occupying cells.
	
}




void GujaratWorld::updateRainfall()
{		
	_climate.step();
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
				setValue(eResources, index, getValue(eMoisture, index));
								
				if(getValue(eResourceType, index)==WILD)
				{
					continue;
				}
				
				if(getValue(eResourceType, index)==SEASONALFALLOW)
				{
					setValue(eResourceType, index, DOMESTICATED);
				}
				int consecutiveYears = getValue(eConsecutiveYears, index);
				consecutiveYears++;				
				if(consecutiveYears<3)
				{
					setValue(eConsecutiveYears, index, consecutiveYears);
				}
				else
				{
					setValue(eConsecutiveYears, index, 0);
					if(getValue(eResourceType, index)==FALLOW)
					{
						setValue(eResourceType, index, WILD);

					}
					else
					{
						setValue(eResourceType, index, FALLOW);
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
				if(getValue(eResourceType, index)==DOMESTICATED)
				{
					setValue(eResourceType, index, SEASONALFALLOW);
				}
			}
		}
	}
}


//void GujaratWorld::updateResourcesFOO()
//{
	
	/*
	if(vFOO.size()==0)
		{
			vFOO.resize(_boundaries._size._x * _boundaries._size._y);
		}

	for(int i = 0; i < _boundaries._size._x * _boundaries._size._y; i++)
	{
		vFOO[i]=2000;
	}
	*/
	
//	for(int i = 0; i < _boundaries._size._x * _boundaries._size._y; i++)
//	{
//		_vFOO[i]=2000;
//	}
	
//}

void GujaratWorld::updateResources()
{
	std::stringstream logName;
	logName << "updateResources_" << _simulation.getId();
	log_DEBUG(logName.str(), getWallTime() << " initiating");

	//unsigned long sumRes = 0;
	
	Seasons season = _climate.getSeason();
	bool wetSeason = false;
	if(season==HOTWET)
	{
		wetSeason = true;
	}			
	Engine::Point2D<int> index;
	for( index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++ )		
	{
		for( index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++ )
		{
			//log_DEBUG(logName.str(), getWallTime() << " index: " << index << " with boundaries: " << _boundaries);
			// 3. Increment or Decrement cell biomass depending on yearly biomass
			//    figures and current timestep
			int currentValue = getValue(eResources, index);
			float currentFraction = (float)getValue(eResourcesFraction, index)/100.0f;
			Soils cellSoil = (Soils)getValue(eSoils, index);
			//log_DEBUG(logName.str(), getWallTime() << " index: " << index << " current: " << currentValue << " fraction: " << currentFraction << " cell soil: " << cellSoil);
			if(cellSoil!=WATER)
			{
				float newValue = std::max(0.0f, currentValue+currentFraction+getBiomassVariation(wetSeason, cellSoil, index));
				currentValue = newValue;
				float fraction = 100.0f*(newValue  - currentValue);
				//log_DEBUG(logName.str(), getWallTime() << " newValue: " << currentValue << " fraction: " << fraction);
				
				setValue(eResources, index, currentValue);
				setValue(eResourcesFraction, index, (int)fraction);
				
				//sumRes += currentValue;
			}
		}
	}
	log_DEBUG(logName.str(), getWallTime() << " end of updateResources");
	
	//std::cout << "sumres: " << sumRes << std::endl;
}

void GujaratWorld::recomputeYearlyBiomass()
{
	
	// update all the map resources to the minimum of the year (in case it was diminished by agents)
	Engine::Point2D<int> index;
	for( index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++ )                
	{
		for( index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++ )
		{
			setValue(eResourcesFraction, index, 0);
			setValue(eResources, index, _remainingBiomass[getValue(eSoils, index)]);
		}
	}

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
	
	//std::cout << std::setprecision(2) << std::fixed << "area of cell: " << areaOfCell << " interdune biomass: " << _config._interduneBiomass << " efficiency: " << _config._interduneEfficiency << " raininess factor: " << raininessFactor << " yearly biomass: " << _yearlyBiomass[INTERDUNE] << std::endl;
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " rain: " << _climate.getRain() << " with mean: " << _climate.getMeanAnnualRain());
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " yearly biomass of dune: " << _yearlyBiomass[DUNE] << " and interdune: " << _yearlyBiomass[INTERDUNE]);

	// yearly biomass is the area of a triangle with max height at the end of wet season
	// A_1 + A_2 = biomass, being A_1 = daysPerSeason*h/2 and A_2 = 2*daysPerSeason*h/2
	// dPS*h/2 + 2*dPS*h/2 = biomass, so h = biomass/1.5*dPS
	// and A_2 = 2*A_1

	double heightInterDune = (_yearlyBiomass[INTERDUNE] + 120.0f*_config._interduneMinimum-60.0*_remainingBiomass[INTERDUNE]) / (180+240*_config._interduneMinimum);
	_dailyRainSeasonBiomassIncrease[INTERDUNE] = (heightInterDune-_remainingBiomass[INTERDUNE])/120.0f;
	_remainingBiomass[INTERDUNE] = heightInterDune *_config._interduneMinimum;
	_dailyDrySeasonBiomassDecrease[INTERDUNE] = heightInterDune*(1-_config._interduneMinimum)/240.0f;

	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " biomass height interdune: " << heightInterDune << " increment: " << _dailyRainSeasonBiomassIncrease[INTERDUNE] << " and decrease: " << _dailyDrySeasonBiomassDecrease[INTERDUNE]);
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " calories height interdune: " << _config._massToEnergyRate*_config._energyToCalRate*heightInterDune << " increment: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyRainSeasonBiomassIncrease[INTERDUNE] << " and decrease: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyDrySeasonBiomassDecrease[INTERDUNE]);

	double heightDune = (_yearlyBiomass[DUNE] + 120.0f*_config._duneMinimum-60.0*_remainingBiomass[DUNE]) / (180+240*_config._duneMinimum);
	_dailyRainSeasonBiomassIncrease[DUNE] = (heightDune-_remainingBiomass[DUNE])/120.0f;
	_remainingBiomass[DUNE] = heightDune *_config._duneMinimum;
	_dailyDrySeasonBiomassDecrease[DUNE] = heightDune*(1-_config._duneMinimum)/240.0f;

	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " biomass height dune: " << heightDune << " increment: " << _dailyRainSeasonBiomassIncrease[DUNE] << " and decrease: " << _dailyDrySeasonBiomassDecrease[DUNE]);
	log_INFO(logName.str(), getWallTime() << " timestep: " << getCurrentTimeStep() << " year: " << getCurrentTimeStep()/360 << " calories height dune: " << _config._massToEnergyRate*_config._energyToCalRate*heightDune << " increment: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyRainSeasonBiomassIncrease[DUNE] << " and decrease: " << _config._massToEnergyRate*_config._energyToCalRate*_dailyDrySeasonBiomassDecrease[DUNE]);

	_dailyRainSeasonBiomassIncrease[WATER] = 0.0f;
	_dailyDrySeasonBiomassDecrease[WATER] = 0.0f;
	_remainingBiomass[WATER] = 0.0f;
	
	//std::cout << "rem: " << _remainingBiomass[DUNE] << " " << _remainingBiomass[INTERDUNE] << " " << _remainingBiomass[WATER] << std::endl;
	/*
	std::cout << "increase: " << _dailyRainSeasonBiomassIncrease[DUNE] << " " 
				<< _dailyRainSeasonBiomassIncrease[INTERDUNE] << " " 
				<< _dailyRainSeasonBiomassIncrease[WATER] << std::endl;
	
	std::cout << "decrease: " << _dailyDrySeasonBiomassDecrease[DUNE] << " " 
				<< _dailyDrySeasonBiomassDecrease[INTERDUNE] << " " 
				<< _dailyDrySeasonBiomassDecrease[WATER] << std::endl;
	*/
}


void GujaratWorld::updateResourcesLowResMap()
{
	fillLRRaster(eLRResources,0);
	// eResources --> eLRResources
	LowResRasterCountsHighResRaster(eLRResources, eResources);
	// eLRResources --> paintLRResources
	//fillHRRasterWithLRRaster(eLRResources, paintLRResources);
	// Dont you add the Fraction Raster of resources?? 
	// LR rasters are used in decision-making, maybe such precission is unneeded.
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
	//TODO look where Min Max Values are used to remove "update'em" if needed
	getDynamicRaster(eResources).updateCurrentMinMaxValues();
	updateResourcesLowResMap();
	getDynamicRaster(eLRResources).updateCurrentMinMaxValues();	
	
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
		variation = _config._waterDistConstant*getValue(eWeightWater, index);
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
	
	//std::cout << "variation: " << variation << std::endl;
	
	return variation;
}


	
void GujaratWorld::worldCell2LowResCell( const Engine::Point2D<int> pos, Engine::Point2D<int> & result ) const
{
	result._x = pos._x / _config._lowResolution;
	result._y = pos._y / _config._lowResolution;
}
	
void GujaratWorld::LowRes2HighResCellCorner(Engine::Point2D<int> pos, Engine::Point2D<int> &result ) const	
{
	result._x = pos._x * _config._lowResolution;
	result._y = pos._y * _config._lowResolution;
}	
	
	
int GujaratWorld::getLowResMapsSideSize() 
{ 
	return _overlapBoundaries._size._x / _config._lowResolution; 
}
	


//*********************************************************

void GujaratWorld::updateeSectorUtility(const std::vector< Engine::Point2D<int> > & cells, int val)
{
	register int C = getConfig()._lowResolution;
	
	for(unsigned int i = 0; i < cells.size(); i++)
	{
		Engine::Point2D<int> LRxycell = cells[i];
		Engine::Point2D<int> paintCell;
		for(paintCell._x = LRxycell._x*C; paintCell._x < LRxycell._x*C + C; paintCell._x++)	
			for(paintCell._y = LRxycell._y*C; paintCell._y < LRxycell._y*C + C; paintCell._y++)	
				getDynamicRaster(eSectorUtility).setValue(paintCell, val);
				
			
	}
	
}

void GujaratWorld::assignResourceFromSectorsToHR(const std::vector< Engine::Point2D<int> > & cells, int HRValToForce)
{
	register int C = getConfig()._lowResolution;
	
	// HR assignation
	for(unsigned int i = 0; i < cells.size(); i++)
	{
		Engine::Point2D<int> LRxycell = cells[i];
		Engine::Point2D<int> paintCell;
		for(paintCell._x = LRxycell._x*C; paintCell._x < LRxycell._x*C + C; paintCell._x++)	
			for(paintCell._y = LRxycell._y*C; paintCell._y < LRxycell._y*C + C; paintCell._y++)	
				getDynamicRaster(eResources).setValue(paintCell, HRValToForce);
	}
	
	// LR assignation
	HRValToForce = HRValToForce * C * C;
	for(unsigned int i = 0; i < cells.size(); i++)
	{		
		getDynamicRaster(eLRResources).setValue(cells[i], HRValToForce);
	}
		
}

	
	

} // namespace Gujarat

