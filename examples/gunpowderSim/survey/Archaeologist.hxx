
#ifndef __Archaeologist_hxx__
#define __Archaeologist_hxx__

#include <Agent.hxx>
#include <string>
#include <Point2D.hxx>

namespace BattleSim
{

class Archaeologist : public Engine::Agent
{
	int _bulletsGathered; // MpiBasicAttribute
	int _skill; // MpiBasicAttribute

	void move();
	void collectFindings();
	
	void serialize();
    void registerAttributes();
public:
	// todo remove environment from here
	Archaeologist( const std::string & id, int skill );
	virtual ~Archaeologist();

	void updateState();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Archaeologist( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace BattleSim

#endif // __Archaeologist_hxx__

