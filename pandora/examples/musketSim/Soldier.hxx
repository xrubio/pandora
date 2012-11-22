
#ifndef __Soldier_hxx__
#define __Soldier_hxx__

#include <Agent.hxx>
#include <string>

#include "Point2D.hxx"

namespace MusketSim
{

class Soldier : public Engine::Agent
{
	bool _isBlueSide; // MpiBasicAttribute
	int _rank; // MpiBasicAttribute
	// actual stress of this soldier	
	int _stress; // MpiBasicAttribute
	// limit to where the soldier will rout
	int _threshold; // MpiBasicAttribute
	int _fireDistance; // MpiBasicAttribute

	// from 1 to 10, rating of the level friend soldier affects morale
	int _cohesionRating; // MpiBasicAttribute
	// distance at which the soldier is supported by his unit
	int _cohesionDistance; // MpiBasicAttribute

	void updateStress();
	void move();
	void rout();
	void fire();
	
	//bool checkDeath();
	void registerAttributes();
	void serialize();
	// if the unit is moving or not
	bool _moving; // MpiBasicAttribute
	bool _routing; // MpiBasicAttribute

	// the seconds that this agent spends in reloading the musket
	int _reloadingTime; // MpiBasicAttribute
	// the seconds to fire again
	int _timeUntilLoaded; // MpiBasicAttribute

	// capability of impacting an enemy, from 1 to 100
	int _accuracy; // MpiBasicAttribute

	// 0 if the soldier has to fire as soon as possible, if not, the number of seconds until fire
	int _delayBeforeFirstFire; // MpiBasicAttribute

	void casualty();
public:
	// todo remove environment from here
	Soldier( const std::string & id, bool isBlueSide, const int & rank, const int & cohesionRating, const int & cohesionDistance, const int & accuracy );
	virtual ~Soldier();

	// evaluation of the threat the enemy at distance is for this soldier
	float getThreatRating( const int & distance );
	
	void updateState();
	void * createPackage();
	void addStress( const int & value );
	int getCohesionRating();	
	bool isRouting();
	bool isMoving();
	void setDelayBeforeFirstFire( const int & delayBeforeFirstFire );

	bool firedThisStep();
	bool isBlueSide(){return _isBlueSide;}
	
	Soldier( void * );
	void * fillPackage(); 
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
};

} // namespace MusketSim

#endif // __Soldier_hxx__

