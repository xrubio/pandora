
#ifndef __BattlefieldConfig_hxx__
#define __BattlefieldConfig_hxx__

#include <Config.hxx>
#include <Point2D.hxx>
#include <string>

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
	void loadParams(); 
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

	// distance between opponent battle lines at the beginning of the simulation
	int _initialDistance;

public:
	BattlefieldConfig( const std::string & xmlFile );
	virtual ~BattlefieldConfig();

	friend class Battlefield;
};

} // namespace BattleSim

#endif // __BattlefieldConfig_hxx__

