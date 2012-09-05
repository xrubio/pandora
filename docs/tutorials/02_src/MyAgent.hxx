
#ifndef __MyAgent_hxx__
#define __MyAgent_hxx__

#include <Agent.hxx>
#include <string>

namespace Tutorial
{

class MyAgent : public Engine::Agent
{
	int _gatheredResources;
public:
	MyAgent( const std::string & id );
	virtual ~MyAgent();
	void registerAttributes();

	void updateState();
	void serialize();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	MyAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Tutorial

#endif // __MyAgent_hxx__

