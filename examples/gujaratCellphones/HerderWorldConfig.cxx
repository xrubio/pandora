#include <HerderWorldConfig.hxx>

namespace GujaratCellphones
{

HerderWorldConfig::HerderWorldConfig( const std::string & xmlFile ) : Config(xmlFile)
{
}

HerderWorldConfig::~HerderWorldConfig()
{
}

void HerderWorldConfig::loadParams()
{
    _averageResources = getParamInt("environment/landscape", "averageResources");
    std::string distribution = getParamStr("environment/landscape", "distribution");

    if(distribution=="distance")
    {
        _randomDistribution = eDistance;
    }
    else if(distribution=="increase")
    {
        _randomDistribution = eIncrease;
    }
    else
    {
        _randomDistribution = eRandom;
    }
    _daysDrySeason = getParamInt("environment/climate", "daysDrySeason");
    _rainHistoricalDistribMean = getParamFloat("environment/climate", "meanRain");
    _rainHistoricalDistribStdDev= getParamFloat("environment/climate", "stddev");

	//population
    _numVillages = getParamInt("population/villages", "num");
    _numAgentsPerVillage = getParamInt("population/villages", "agentsPerVillage");
    _animalsPerHerder = getParamInt("population/animals", "numPerHerder");
    _resourcesNeededPerAnimal = getParamInt("population/animals", "resourcesPerDay");

	//mdp
    _horizon = getParamInt("mdp", "horizon");
    _width = getParamInt("mdp", "width");
    _explorationBonus = getParamInt("mdp", "explorationBonus");

	//knowledge transmission
    _inVillageTransmission = getParamBool("knowledgeTransmission/inVillage", "active");
    _inVillageTransmissionValue = getParamInt("knowledgeTransmission/inVillage", "fixedValue");

    _outVillageTransmission = getParamBool("knowledgeTransmission/outVillage", "active");
    _outVillageTransmissionValue = getParamInt("knowledgeTransmission/outVillage", "fixedValue");
}

} // namespace GujaratCellphones

