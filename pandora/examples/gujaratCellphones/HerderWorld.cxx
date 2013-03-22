#include <HerderWorld.hxx>
#include "Herder.hxx"

namespace GujaratCellphones
{

HerderWorld::HerderWorld(Engine::Simulation &simulation, HerderWorldConfig &config) : World(simulation, 1, true, config._resultsFile), _climate(config)
{
	_config = config;
	_dailyDecrease.resize(11);
	_maxResources.resize(11);
}

HerderWorld::~HerderWorld()
{
}

void HerderWorld::createAgents()
{
	for(int i=0; i<_config._numVillages; i++)
	{
		Engine::Point2D<int> villageLocation = Engine::Point2D<int>(Engine::GeneralState::statistics().getUniformDistValue(0, _config._size-1),Engine::GeneralState::statistics().getUniformDistValue(0, _config._size-1));
		std::ostringstream oss;
		oss << "Village_" << i;
		Village * newVillage = new Village(oss.str(), i);
		newVillage->setPosition(villageLocation);

		// in village transmission
		if(_config._inVillageTransmission)
		{
			if(_config._inVillageTransmissionValue==-1)
			{
				newVillage->setInVillageTransmission(Engine::GeneralState::statistics().getUniformDistValue(0, 100));
			}
			else
			{
				newVillage->setInVillageTransmission(_config._inVillageTransmissionValue);
			}
		}
		else
		{
			newVillage->setInVillageTransmission(0);
		}
		
		// mobile transmission
		if(_config._outVillageTransmission)
		{
			if(_config._outVillageTransmissionValue==-1)
			{
				newVillage->setOutVillageTransmission(Engine::GeneralState::statistics().getUniformDistValue(0, 100));
			}
			else
			{
				newVillage->setOutVillageTransmission(_config._outVillageTransmissionValue);
			}
		}
		else
		{
			newVillage->setOutVillageTransmission(0);
		}
		addAgent(newVillage);

		for(int j=0; j< _config._numAgentsPerVillage; j++)
		{
			std::ostringstream ossH;
			ossH << "Herder_"<<j<<"_vil" << i;
			Herder * newHerder = new Herder(ossH.str(), _config._animalsPerHerder, _config._resourcesNeededPerAnimal, *newVillage);
			newHerder->configureMDP(_config._horizon, _config._width, _config._explorationBonus);			

			addAgent(newHerder);
			newHerder->createKnowledge();
		}
	}
}

void HerderWorld::createRasters()
{	
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("soil quality", true, eSoilQuality);
	getDynamicRaster(eSoilQuality).setInitValues(0, 10, 5);

	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			int value = Engine::GeneralState::statistics().getNormalDistValue(0,10);
			getDynamicRaster(eSoilQuality).setMaxValue(index, value);
		}
	}
	updateRasterToMaxValues(eSoilQuality);	
	
	// we need to keep track of resource fractions
	registerDynamicRaster("resourcesFraction", false, eResourcesFraction);
	getDynamicRaster(eResourcesFraction).setInitValues(0, 100, 0);

	registerDynamicRaster("gathered", true);
	getDynamicRasterStr("gathered").setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void HerderWorld::recomputeYearlyBiomass()
{
	float rainWeight = _climate.getRain()/_config._rainHistoricalDistribMean;
	std::cout << " rain: " << _climate.getRain() << " mean: " << _config._rainHistoricalDistribMean << " weight: " << rainWeight << std::endl;
	Engine::Point2D<int> index;

	for(int i=0; i<_maxResources.size(); i++)
	{
		float maxValue = (i*rainWeight*_config._averageResources)/5.0f;
		_maxResources.at(i) = maxValue;
		float decrease = maxValue*1.0f;
		// days of dry season -1 because the first day it is not decreased
		_dailyDecrease.at(i) = decrease/(_config._daysDrySeason-1);

	}

	for(index._x = _overlapBoundaries._origin._x; index._x < _overlapBoundaries._origin._x + _overlapBoundaries._size._x; index._x++ )                
	{
		for( index._y = _overlapBoundaries._origin._y; index._y < _overlapBoundaries._origin._y + _overlapBoundaries._size._y; index._y++ )
		{
			setValue(eResources, index, _maxResources.at(getValue(eSoilQuality, index)));
		}
	}
}

void HerderWorld::updateResources()
{
	Engine::Point2D<int> index;
	for( index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++ )		
	{
		for( index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++ )
		{
			int currentValue = getValue(eResources, index);
			float decrease = _dailyDecrease.at(getValue(eSoilQuality, index));

			float currentFraction = (float)getValue(eResourcesFraction, index)/100.0f;
			float newValue = std::max(0.0f, currentValue+currentFraction-decrease);
			currentValue = newValue;
			float fraction = 100.0f*(newValue  - currentValue);

			setValue(eResources, index, currentValue);
			setValue(eResourcesFraction, index, (int)fraction);
		}
	}
}

bool HerderWorld::isWetSeason() const
{
	if(_step%_config._daysDrySeason==0)
	{
		return true;
	}
	return false;
}

void HerderWorld::stepEnvironment()
{
	// first day of year -> step to include whole wet season
	if(isWetSeason())
	{
		_climate.computeRainValue();
		recomputeYearlyBiomass();
	}
	else
	{
//		updateResources();
	}
}

int HerderWorld::daysUntilWetSeason() const
{
	if(isWetSeason())
	{
		return 0;
	}
	int days = _step%_config._daysDrySeason;
	return _config._daysDrySeason - days;
}
	
const HerderWorldConfig & HerderWorld::getConfig() const
{
	return _config;
}

}

