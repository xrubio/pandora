#include <HerderWorld.hxx>
#include "Herder.hxx"

namespace GujaratCellphones
{

HerderWorld::HerderWorld( HerderWorldConfig * config) : World(config, HerderWorld::useOpenMPSingleNode(), true), _climate(*config)
{
	_maxResources.resize(11);
}

HerderWorld::~HerderWorld()
{
}

void HerderWorld::createAgents()
{
    const HerderWorldConfig & herderConfig = (const HerderWorldConfig &)getConfig();
	for(int i=0; i<herderConfig._numVillages; i++)
	{
		Engine::Point2D<int> villageLocation(herderConfig._size._width/2,herderConfig._size._height/2);
		std::ostringstream oss;
		oss << "Village_" << i;
		Village * newVillage = new Village(oss.str(), i);
		newVillage->setPosition(villageLocation);

		// in village transmission
		if(herderConfig._inVillageTransmission)
		{
			if(herderConfig._inVillageTransmissionValue==-1)
			{
				newVillage->setInVillageTransmission(Engine::GeneralState::statistics().getUniformDistValue(0, 100));
			}
			else
			{
				newVillage->setInVillageTransmission(herderConfig._inVillageTransmissionValue);
			}
		}
		else
		{
			newVillage->setInVillageTransmission(0);
		}
		
		// mobile transmission
		if(herderConfig._outVillageTransmission)
		{
			if(herderConfig._outVillageTransmissionValue==-1)
			{
				newVillage->setOutVillageTransmission(Engine::GeneralState::statistics().getUniformDistValue(0, 100));
			}
			else
			{
				newVillage->setOutVillageTransmission(herderConfig._outVillageTransmissionValue);
			}
		}
		else
		{
			newVillage->setOutVillageTransmission(0);
		}
		addAgent(newVillage);

		for(int j=0; j< herderConfig._numAgentsPerVillage; j++)
		{
			std::ostringstream ossH;
			ossH << "Herder_"<<newVillage->getNewKey()<<"_vil" << i;
			Herder * newHerder = new Herder(ossH.str(), herderConfig._animalsPerHerder, herderConfig._resourcesNeededPerAnimal, *newVillage);
			newHerder->configureMDP(herderConfig._horizon, herderConfig._width, herderConfig._explorationBonus);			

			addAgent(newHerder);
			newHerder->createKnowledge();
		}
	}
}

void HerderWorld::createRasters()
{	
    const HerderWorldConfig & herderConfig = (const HerderWorldConfig &)getConfig();
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("soil quality", true, eSoilQuality);
	getDynamicRaster(eSoilQuality).setInitValues(0, 10, 0);


    int totalValue = 0;
    float cellIncrease = 10.0f/float(getBoundaries()._size._height);
    for(auto index : getBoundaries())
    {
        int value = 0;
        if(herderConfig._randomDistribution == eIncrease)
        {
            value = (cellIncrease*index._y);
        }
        else if(herderConfig._randomDistribution == eDistance)
        {
            Engine::Point2D<int> center(getBoundaries()._size._width/2, getBoundaries()._size._height/2);
            // half is a random value + half is based on distance
		    value = Engine::GeneralState::statistics().getUniformDistValue(0, 5);
            value += cellIncrease*(center.distance(index))/2.0f;
        }
        else
        {
		    value = Engine::GeneralState::statistics().getUniformDistValue(0, 10);
        }
        totalValue += value;
		setMaxValue(eSoilQuality, index, value);
	}
    std::cout << "total value: " << totalValue << std::endl;
	updateRasterToMaxValues(eSoilQuality);
    recomputeYearlyBiomass();
}

void HerderWorld::recomputeYearlyBiomass()
{
    const HerderWorldConfig & herderConfig = (const HerderWorldConfig &)getConfig();
	float rainWeight = _climate.getRain()/herderConfig._rainHistoricalDistribMean;
	std::cout << " rain: " << _climate.getRain() << " mean: " << herderConfig._rainHistoricalDistribMean << " weight: " << rainWeight << std::endl;
	Engine::Point2D<int> index;

	for(int i=0; i<_maxResources.size(); i++)
	{
		float maxValue = (i*rainWeight*herderConfig._averageResources)/5.0f;
		_maxResources.at(i) = maxValue;

	}

    for(auto index : getBoundaries())
    {
		setValue(eResources, index, _maxResources.at(getValue(eSoilQuality, index)));
	}
}

bool HerderWorld::isWetSeason() const
{
    const HerderWorldConfig & herderConfig = (const HerderWorldConfig &)getConfig();
	if(_step%herderConfig._daysDrySeason==0)
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
    const HerderWorldConfig & herderConfig = (const HerderWorldConfig &)getConfig();
	if(isWetSeason())
	{
		return 0;
	}
	int days = _step%herderConfig._daysDrySeason;
	return herderConfig._daysDrySeason - days;
}
	
}

