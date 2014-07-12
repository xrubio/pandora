
#ifndef __MDPAgent_hxx__
#define __MDPAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class MDPAgent : public Engine::Agent
{
	int _resources; // MpiBasicAttribute

public:
	// todo remove environment from here
	MDPAgent( const std::string & id );
	virtual ~MDPAgent();
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

	void setResources( int resources );
	int getResources() const;
    int getNeededResources(){return 1;}

};

} // namespace Examples

#endif // __MDPAgent_hxx__

