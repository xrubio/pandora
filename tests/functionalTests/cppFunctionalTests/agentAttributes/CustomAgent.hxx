
#ifndef __CustomAgent_hxx__
#define __CustomAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Test
{

class CustomAgent : public Engine::Agent
{
	std::string _strAttribute; // MpiStringAttribute
	int _intAttribute; // MpiBasicAttribute
	float _floatAttribute; // MpiBasicAttribute

public:
	// todo remove environment from here
	CustomAgent( const std::string & id );
	virtual ~CustomAgent();
	
	void updateState();
	void registerAttributes();
	void serialize();

	void setStringAttribute( const std::string & attribute );
	void setIntAttribute( int intAttribute );
	void setFloatAttribute( float floatAttribute );

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	CustomAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Test

#endif // __CustomAgent_hxx__

