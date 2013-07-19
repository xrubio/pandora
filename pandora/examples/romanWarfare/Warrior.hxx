
#ifndef __Warrior_hxx__
#define __Warrior_hxx__

#include <Agent.hxx>

#include <string>
#include <Point2D.hxx>

namespace ClassicalWarfare
{

class Warrior : public Engine::Agent
{
	/*
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
	*/
	
	void serialize();
	void registerAttributes();

	/*
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
	*/
public:
	// todo remove environment from here
	Warrior( const std::string & id/*, const int & rank, const int & cohesionRating, const int & cohesionDistance, const int & accuracy, const int & reloadingTime*/ );
	virtual ~Warrior();

	/*
	// evaluation of the threat the enemy at distance is for this soldier
	float getThreatRating( const int & distance );
	*/
	void updateState();
	/*
	void addStress( const int & value );
	int getCohesionRating();	
	bool isRouting();
	bool isMoving();
	void setDelayBeforeFirstFire( const int & delayBeforeFirstFire );

	bool firedThisStep();
	*/


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	Warrior( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace ClassicalWarfare

#endif // __Warrior_hxx__

