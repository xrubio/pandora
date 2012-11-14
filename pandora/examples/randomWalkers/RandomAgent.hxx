
#ifndef __RandomAgent_hxx__
#define __RandomAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class RandomAgent : public Engine::Agent
{
	int _resources; // MpiBasicAttribute

public:
	// todo remove environment from here
	RandomAgent( const std::string & id );
	virtual ~RandomAgent();
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

	void setResources( int resources );
	int getResources() const;
};

} // namespace Examples

#endif // __RandomAgent_hxx__

