
#ifndef __Soldier_hxx__
#define __Soldier_hxx__

#include "Agent.hxx"
#include <string>

#include "Point2D.hxx"

namespace Engine
{
	class Agent;
}

namespace BattleSim
{

typedef struct
{
	char _id[100];	
	Engine::Point2D<int> _position;
	bool _isBlueSide;
	int _stress;
	int _cohesionRating;
	int _cohesionDistance;
	int _rank;
	bool _moving;
	bool _routing;
	int _reloadingTime;
	int _timeUntilLoaded;
	int _accuracy;
} SoldierPackage;

class Soldier : public Engine::Agent
{
	bool _isBlueSide;
	int _rank;
	// actual stress of this soldier	
	int _stress;
	// limit to where the soldier will rout
	int _threshold;
	int _fireDistance;

	// from 1 to 10, rating of the level friend soldier affects morale
	int _cohesionRating;
	// distance at which the soldier is supported by his unit
	int _cohesionDistance;

	void updateStress();
	void move();
	void rout();
	void fire();
	
	//bool checkDeath();
	void serialize();
	// if the unit is moving or not
	bool _moving;
	bool _routing;

	// the seconds that this agent spends in reloading the musket
	int _reloadingTime;
	// the seconds to fire again
	int _timeUntilLoaded;

	// capability of impacting an enemy, from 1 to 100
	int _accuracy;

	// 0 if the soldier has to fire as soon as possible, if not, the number of seconds until fire
	int _delayBeforeFirstFire;

	void casualty();
public:
	// todo remove environment from here
	Soldier( const std::string & id, const int & rank, const int & cohesionRating, const int & cohesionDistance, const int & accuracy );
	Soldier( const SoldierPackage & package );
	virtual ~Soldier();

	// evaluation of the threat the enemy at distance is for this soldier
	float getThreatRating( const int & distance );
	
	void step();
	void * createPackage();
	void addStress( const int & value );
	int getCohesionRating();	
	bool isRouting();
	bool isMoving();
	void setDelayBeforeFirstFire( const int & delayBeforeFirstFire );

	bool firedThisStep();
};

} // namespace BattleSim

#endif // __Soldier_hxx__

