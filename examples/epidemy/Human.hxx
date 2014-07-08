
#ifndef __Human_hxx__
#define __Human_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class Human : public Engine::Agent
{
	float _threatLevel; // MpiAttribute

public:
	// todo remove environment from here
	Human( const std::string & id );
	virtual ~Human();
	
	void updateKnowledge();
	void selectActions();
	void updateState();
	
	void registerAttributes();
	void serialize();

	// MPI related
	Human( void * );
	void * fillPackage(); 
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
};

} // namespace Examples

#endif // __Human_hxx__

