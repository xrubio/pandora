
#ifndef __BattlefieldConfig_hxx__
#define __BattlefieldConfig_hxx__

#include "Config.hxx"
#include <string>

class TiXmlElement;

enum Tactics
{
	eVolley = 1,
	eRanks = 2,
	ePlatoon = 3,
	eCatalan = 4
};

namespace BattleSim
{



class BattlefieldConfig : public Config
{
	void extractParticularAttribs(TiXmlElement * root);
public:
	int _numBlueSoldiers;
	int _blueRanks;
	int _blueCohesionRating;
	int _blueCohesionDistance;
	int _blueAccuracy;
	Tactics _blueTactics;

	int _numRedSoldiers;
	int _redRanks;
	int _redCohesionRating;
	int _redCohesionDistance;
	int _redAccuracy;
	Tactics _redTactics;

	int _size;
public:
	BattlefieldConfig();
	virtual ~BattlefieldConfig();
};

} // namespace BattleSim

#endif // __BattlefieldConfig_hxx__

