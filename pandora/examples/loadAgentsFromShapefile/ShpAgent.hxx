
#ifndef __ShpAgent_hxx__
#define __ShpAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class ShpAgent : public Engine::Agent
{
	std::string _label;
	int _intValue;
	float _floatValue;

public:
	// todo remove environment from here
	ShpAgent( const std::string & id );
	virtual ~ShpAgent();
	
	void updateState();
	void serialize();

	void setLabel( const std::string & label );
	void setIntValue( int intValue );
	void setFloatValue( float floatValue );

	const std::string getLabel();
	int getIntValue();
	float getFloatValue();

	// Mpì
	ShpAgent( void * );
	void * fillPackage(); 
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
};

} // namespace Examples

#endif // __ShpAgent_hxx__

