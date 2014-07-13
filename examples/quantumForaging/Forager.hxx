#ifndef __Forager_hxx__
#define __Forager_hxx__

#include <Agent.hxx>
#include <engine/policy.h>
#include <engine/uct.h>
#include "ForagerState.hxx"

namespace QuantumExperiment
{

class DecisionModel;

class Forager : public Engine::Agent
{
	typedef Online::Policy::random_t< ForagerState> BasePolicy;
	typedef Online::Policy::UCT::uct_t< ForagerState > UCT;

	int _currentResources; // MpiBasicAttribute
	int _neededResources; // MpiBasicAttribute
	bool _perfectInformation; // MpiBasicAttribute
	// fitness, in terms of % of time steps where the agent didn't get needed resources
	float _starvation; // MpiBasicAttribute

	DecisionModel * _model;
	BasePolicy * _uctBasePolicy;
	// mdp
	int _horizon; // MpiBasicAttribute
	int _width; // MpiBasicAttribute
	int _explorationBonus; // MpiBasicAttribute

	//mental map
	std::string _knowledgeMap; //quality of knowledge, from 0% (no idea) to 100% (perfect idea)
	std::string _resourcesMap; //value of resources

	// decision making process
	float _riskAversion; // MpiBasicAttribute
	float _ambiguityAversion; // MpiBasicAttribute
	int _moveActions; // MpiBasicAttribute
		
public:
	Forager( const std::string & id, int neededResources, bool perfectInformation, float riskAversion );
	virtual ~Forager();

	//sets and gets
	void setCurrentResources( int currentResources );
	int getCurrentResources() const;
	int getNeededResources() const;
	int getHorizon() const;
	const std::string & getKnowledgeMap() const;
	const std::string & getResourcesMap() const;

	//mental map
	void createKnowledge();
	void updateKnowledge();

	//mdp
	void configureMDP( const int & horizon, const int & width, const int & explorationBonus );
	void selectActions();
	void updateState();

	//register information
	void registerAttributes();
	void serialize();

	float getRiskAversion() const;
	float getAmbiguityAversion() const;

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Forager( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace QuantumExperiment

#endif // __Forager_hxx__

