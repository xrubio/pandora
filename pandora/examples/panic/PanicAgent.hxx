
#ifndef __PanicAgent_hxx__
#define __PanicAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Panic
{

class PanicAgent : public Engine::Agent
{
	int _direction;
	bool _exited;
	Engine::Point2D<int> _exit;
	Engine::Point2D<float> _rest;

	Engine::Point2D<float> getNextPos( const int & direction, const Engine::Point2D<float> & position );
	float getDistToNearestObstacle( const int & direction );
public:
	// todo remove environment from here
	PanicAgent( const std::string & id );
	virtual ~PanicAgent();
	void setExit( const Engine::Point2D<int> & exit );
	
	void updateState();
	void registerAttributes();
	void serialize();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	PanicAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Panic

#endif // __PanicAgent_hxx__

