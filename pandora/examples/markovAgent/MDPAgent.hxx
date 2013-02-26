
#ifndef __MDPAgent_hxx__
#define __MDPAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include "MDPAgentModel.hxx"
#include "MDPAgentState.hxx"

#include <engine/policy.h>
#include <engine/uct.h>
#include <string>

namespace Examples
{

class MDPAgent : public Engine::Agent
{
	typedef Online::Policy::random_t< MDPAgentState> BasePolicy;
	typedef Online::Policy::UCT::uct_t< MDPAgentState > UCT;

	int _resources; // MpiBasicAttribute
	MDPAgentModel * _model;
	BasePolicy * _uctBasePolicy;

	// mdp
	int _horizon;
	int _width;
	int _explorationBonus;
public:
	// todo remove environment from here
	MDPAgent( const std::string & id, const int & horizon, const int & width, const int & explorationBonus);
	virtual ~MDPAgent();
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

	void setResources( int resources );
	int getResources() const;

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	MDPAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __MDPAgent_hxx__

