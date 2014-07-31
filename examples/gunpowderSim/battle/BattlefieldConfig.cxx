
#include "BattlefieldConfig.hxx"

namespace BattleSim
{

BattlefieldConfig::BattlefieldConfig( const std::string & xmlFile ) : Config(xmlFile), _numBlueSoldiers(0), _blueRanks(0), _blueCohesionRating(0), _blueCohesionDistance(0), _blueAccuracy(0), _blueReloadingTime(0), _numRedSoldiers(0), _redRanks(0), _redCohesionRating(0), _redCohesionDistance(0), _redAccuracy(0), _redReloadingTime(0)
{
}

void BattlefieldConfig::loadParams()
{
    _initialDistance = getParamInt("battlefield", "initialDistance");

    _numBlueSoldiers = getParamInt("blueSide", "numSoldiers");
    _blueRanks = getParamInt("blueSide", "ranks");
    _blueCohesionRating = getParamInt("blueSide", "cohesionRating");
    _blueCohesionDistance = getParamInt("blueSide", "cohesionDistance");
    _blueAccuracy = getParamInt("blueSide", "accuracy");
    _blueTactics = (Tactics)getParamInt("blueSide", "tactic");
    _blueReloadingTime = getParamInt("blueSide", "reloadingTime");
    
    _numRedSoldiers = getParamInt("redSide", "numSoldiers");
    _redRanks = getParamInt("redSide", "ranks");
    _redCohesionRating = getParamInt("redSide", "cohesionRating");
    _redCohesionDistance = getParamInt("redSide", "cohesionDistance");
    _redAccuracy = getParamInt("redSide", "accuracy");
    _redTactics = (Tactics)getParamInt("redSide", "tactic");
    _redReloadingTime = getParamInt("redSide", "reloadingTime");

}

BattlefieldConfig::~BattlefieldConfig()
{
}

} // namespace BattleSim

