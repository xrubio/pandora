
#ifndef __Bullet_hxx__
#define __Bullet_hxx__

#include "Agent.hxx"
#include <string>

#include "Point2D.hxx"

namespace BattleSim
{

typedef struct
{
	char _id[100];	
	Engine::Point2D<int> _position;
	bool _isBlueSide;
	int _impactProbability;
} BulletPackage;

class Bullet : public Engine::Agent
{
	// bullets are being fired at 150 height, 457m/s muzzle velocity and 0 degrees of inclination
	bool _isBlueSide;
	// velocity in m/s, from sources
	int _velocity;
	// falling time t = sqrt(2*initial height/9.8)
	float _fallingTime;
	// from 0 to 100, probability of impacting a soldier
	int _impactProbability;

	void serialize();
	// if true, bullet will continue moving
	bool checkContinue( const Engine::Point2D<int> & newPos );

public:
	Bullet( const std::string & id, const int & impactProbability );
	Bullet( const BulletPackage & package );
	virtual ~Bullet();

	void step();
	void * createPackage();
	// this function will calculate initial velocity taking standardVelocity as a median, and creating a normal distribution of ranges
	void setMuzzleVelocity( const int & standardVelocity );

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	Bullet( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace BattleSim

#endif // __Bullet_hxx__

