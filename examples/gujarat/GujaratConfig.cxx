#include <GujaratConfig.hxx>
#include <sstream>
#include <Exception.hxx>
#include <HunterGathererMDPConfig.hxx>

#include <GujaratState.hxx>

namespace Gujarat
{

GujaratConfig::GujaratConfig( const std::string & xmlFile ) : Config(xmlFile), _soilFile("soil file not loaded"), _demFile("dem file not loaded"), _distWaterFile("distance to water file not loaded"), _weightWaterFile("weight to water file not loaded"), _climateSeed(1), _hunterGathererController( "Rule-Based" ), _numHG(0)
{
}
  
GujaratConfig::~GujaratConfig()
{

}

void GujaratConfig::loadParams()
{
    _massToEnergyRate = getParamFloat("massToEnergyRate", "value");
    _energyToCalRate = getParamFloat("energyToCaloriesRate", "value");

    _socialRange = getParamInt("socialRange", "value");
    _daysPerSeason = getParamInt("daysPerSeason", "value");
    _daysPerYear = getParamInt("daysPerYear", "value");
   
    _cellResolution = getParamFloat("cellResolution", "value");
    _lowResolution = getParamInt("lowResolution", "value");

    _duneBiomass = getParamInt("cellDune", "value");
    _duneEfficiency = getParamFloat("cellDune", "efficiency");
    _duneMinimum = getParamFloat("cellDune", "minimum");
    _interduneBiomass = getParamInt("cellInterdune", "value");
    _interduneEfficiency = getParamFloat("cellInterdune", "efficiency");
    _interduneMinimum = getParamFloat("cellInterdune", "minimum");

    _biomassDistribution  = getParamStr("biomassDistribution", "type");
    _distWaterFile = getParamStr("biomassDistribution", "distToWaterFile");

    if(_biomassDistribution!="standard")
    {
        std::stringstream path;
        path << "biomassDistribution/" << _biomassDistribution;
        _weightWaterFile = getParamStr(path.str(), "weightFile");
	    long int sumWeights = getParamLongInt(path.str(), "sumWeights");
	    int numCells = getParamInt("biomassDistribution", "size");
        numCells *= numCells;
		_waterDistConstant = (double)numCells/(double)sumWeights;
    }

    _soilFile = getParamStr("soil", "fileName");
    _demFile = getParamStr("dem", "fileName");

    _climateSeed = getParamInt("climateSeed", "value");
    _rainHistoricalDistribMean = getParamFloat("rainHistoricalDistribution", "mean");
    _rainHistoricalDistribStdev = getParamFloat("rainHistoricalDistribution", "stdev");

    // hunter-gatherers
    _numHG = getParamInt("hunterGatherers", "num");
    _homeRange = getParamInt("hunterGatherers", "homeRange");
    _adulthoodAge = getParamInt("hunterGatherers", "adulthoodAge");
    _numSectors = getParamInt("hunterGatherers", "numSectors");
    _walkingSpeedHour = getParamFloat("hunterGatherers", "walkingSpeedHour");
    _forageTimeCost = getParamFloat("hunterGatherers", "forageTimeCost");

    _hunterGathererController = getParamStr("hunterGatherers", "controllerType");

    // calories
	float minValue = getParamFloat("hunterGatherers/calories", "minValue");
	float adultValue = getParamFloat("hunterGatherers/calories", "adultValue");
	int minAge = getParamFloat("hunterGatherers/calories", "minAge");
	int adultAge = getParamFloat("hunterGatherers/calories", "adultAge");
	GujaratState::setHGCaloricRequirements(minAge, adultAge, minValue, adultValue);

    minValue = getParamFloat("hunterGatherers/availableForageTime", "minValue");
	adultValue = getParamFloat("hunterGatherers/availableForageTime", "adultValue");
	minAge = getParamFloat("hunterGatherers/availableForageTime", "minAge");
	adultAge = getParamFloat("hunterGatherers/availableForageTime", "adultAge");
	GujaratState::setHGAvailableForageTime(minAge, adultAge, minValue, adultValue);

    _storeRasters["soils"] = getParamBool("soilStore", "store");
    _storeRasters["dem"] = getParamBool("soilStore", "store");
    _storeRasters["resources"] = getParamBool("soilStore", "store");
    _storeRasters["forageActivity"] = getParamBool("soilStore", "store");
    _storeRasters["homeActivity"] = getParamBool("soilStore", "store");
    _storeRasters["sectors"] = getParamBool("soilStore", "store");

    _controllerConfig._nrForageActions = getParamInt("hunterGatherers/mdpConfig", "nrForageActions");
    _controllerConfig._nrMoveHomeActions = getParamInt("hunterGatherers/mdpConfig", "nrMoveHomeActions");
    _controllerConfig._doNothingAllowed = getParamBool("hunterGatherers/mdpConfig", "doNothingIsAllowed");
    _controllerConfig._horizon = getParamInt("hunterGatherers/mdpConfig", "horizon");
    _controllerConfig._width = getParamInt("hunterGatherers/mdpConfig", "width");
    _controllerConfig._explorationBonus = -1.0f*getParamFloat("hunterGatherers/mdpConfig", "explorationBonus");
    GujaratState::initializeSectorsMask(_numSectors, _homeRange);
    GujaratState::setHGController( _hunterGathererController, _controllerConfig);
}
 
} // namespace Gujarat

