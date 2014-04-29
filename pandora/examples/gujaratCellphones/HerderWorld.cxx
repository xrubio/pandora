#include <HerderWorld.hxx>
#include "Herder.hxx"

namespace GujaratCellphones
{

HerderWorld::HerderWorld(Engine::Simulation &simulation, HerderWorldConfig &config) : World(simulation, HerderWorld::useOpenMPSingleNode(config._resultsFile), true), _climate(config)
{
	_config = config;
	_maxResources.resize(11);
}

HerderWorld::~HerderWorld()
{
}

void HerderWorld::createAgents()
{
	for(int i=0; i<_config._numVillages; i++)
	{
		Engine::Point2D<int> villageLocation(_config._size._width/2,_config._size._height/2);
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
	getDynamicRaster(eSoilQuality).setInitValues(0, 10, 0);

    float cellIncrease = 10.0f/float(getBoundaries()._size._height);

    for(auto index : getBoundaries())
    {
		int value = (cellIncrease*index._y);
		getDynamicRaster(eSoilQuality).setMaxValue(index, value);
	}
	updateRasterToMaxValues(eSoilQuality);
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

	}

    for(auto index : getBoundaries())
    {
		setValue(eResources, index, _maxResources.at(getValue(eSoilQuality, index)));
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

