
#include "BattlefieldConfig.hxx"
#include <tinyxml.h>

namespace BattleSim
{

BattlefieldConfig::BattlefieldConfig() : _numBlueSoldiers(0), _blueRanks(0), _blueCohesionRating(0), _blueCohesionDistance(0), _blueAccuracy(0), _blueReloadingTime(0), _numRedSoldiers(0), _redRanks(0), _redCohesionRating(0), _redCohesionDistance(0), _redAccuracy(0), _redReloadingTime(0), _size(0)
{
}

void BattlefieldConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("blueSide");
	_numBlueSoldiers = atoi(element->Attribute("numSoldiers"));
	_blueRanks = atoi(element->Attribute("ranks"));
	_blueCohesionRating = atoi(element->Attribute("cohesionRating"));
	_blueCohesionDistance = atoi(element->Attribute("cohesionDistance"));
	_blueAccuracy = atoi(element->Attribute("accuracy"));
	_blueReloadingTime = atoi(element->Attribute("reloadingTime"));
	_blueTactics = (Tactics)atoi(element->Attribute("tactic"));

	element = root->FirstChildElement("redSide");
	_numRedSoldiers = atoi(element->Attribute("numSoldiers"));
	_redRanks = atoi(element->Attribute("ranks"));
	_redCohesionRating = atoi(element->Attribute("cohesionRating"));
	_redCohesionDistance = atoi(element->Attribute("cohesionDistance"));
	_redAccuracy = atoi(element->Attribute("accuracy"));
	_redReloadingTime = atoi(element->Attribute("reloadingTime"));
	_redTactics = (Tactics)atoi(element->Attribute("tactic"));
	
	element = root->FirstChildElement("battlefield");
	_size = atoi(element->Attribute("size"));
	_initialDistance = atoi(element->Attribute("initialDistance"));
}

BattlefieldConfig::~BattlefieldConfig()
{
}

} // namespace BattleSim

