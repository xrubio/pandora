
#ifndef __HelloAgent_hxx__
#define __HelloAgent_hxx__

#include <Agent.hxx>
#include <string>

class HelloAgent : public Engine::Agent
{
	int _value; // MpiBasicAttribute

public:
	// todo remove environment from here
	HelloAgent( const std::string & id );
	virtual ~HelloAgent();
	
	void updateState();
	void registerAttributes();
	void serialize();


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	HelloAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

#endif // __HelloAgent_hxx__

