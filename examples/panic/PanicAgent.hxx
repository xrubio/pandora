
#ifndef __PanicAgent_hxx__
#define __PanicAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Panic
{
class ScenarioConfig;

class PanicAgent : public Engine::Agent
{
	int _direction;
	bool _exited;
	bool _panicked;
	Engine::Point2D<int> _exit;
	Engine::Point2D<float> _rest;

	Engine::Point2D<float> getNextPos( const int & direction, const Engine::Point2D<float> & position );
	float getDistToNearestObstacle( const int & direction );
	float getCompressionLevel( float direction );

	int _consecutive;

public:
	// todo remove environment from here
	PanicAgent( const std::string & id);
	virtual ~PanicAgent();
	void setExit( const Engine::Point2D<int> & exit );
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	PanicAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Panic

#endif // __PanicAgent_hxx__

