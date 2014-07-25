
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
    bool _infected; // MpiAttribute

    // infected related
    bool _hasHumans; // MpiAttribute
	int _remainingTime; // MpiAttribute

public:
	// todo remove environment from here
	Human( const std::string & id, bool infected = false);
	virtual ~Human();
	
	void updateKnowledge();
	void selectActions();
	void updateState();
	
	void registerAttributes();
	void serialize();

    bool isInfected() const { return _infected;}


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Human( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __Human_hxx__

