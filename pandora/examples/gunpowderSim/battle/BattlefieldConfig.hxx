
#ifndef __BattlefieldConfig_hxx__
#define __BattlefieldConfig_hxx__

#include <Config.hxx>
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
class Battlefield;

class BattlefieldConfig : public Engine::Config
{
	void extractParticularAttribs(TiXmlElement * root);
public:
	int _numBlueSoldiers;
	int _blueRanks;
	int _blueCohesionRating;
	int _blueCohesionDistance;
	int _blueAccuracy;
	int _blueReloadingTime;
	Tactics _blueTactics;

	int _numRedSoldiers;
	int _redRanks;
	int _redCohesionRating;
	int _redCohesionDistance;
	int _redAccuracy;
	int _redReloadingTime;
	Tactics _redTactics;

	int _size;
	// distance between opponent battle lines at the beginning of the simulation
	int _initialDistance;

public:
	BattlefieldConfig();
	virtual ~BattlefieldConfig();

	friend class Battlefield;
};

} // namespace BattleSim

#endif // __BattlefieldConfig_hxx__

