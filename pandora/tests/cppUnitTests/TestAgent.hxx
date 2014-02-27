
#ifndef __TestAgent_hxx__
#define __TestAgent_hxx__

#include <Agent.hxx>
#include <string>

namespace Test
{

class TestAgent : public Engine::Agent
{
public:
	TestAgent( const std::string & id );
	virtual ~TestAgent();
	void registerAttributes();

	void updateState();
	void serialize();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	TestAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Test

#endif // __TestAgent_hxx__

