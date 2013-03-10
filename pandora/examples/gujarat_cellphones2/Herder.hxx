
#ifndef __Herder_hxx__
#define __Herder_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include "DecisionModel.hxx"
#include "HerderState.hxx"

#include <engine/policy.h>
#include <engine/uct.h>
#include <string>

namespace GujaratCellphones
{

class Village;

class Herder : public Engine::Agent
{
	typedef Online::Policy::random_t< HerderState> BasePolicy;
	typedef Online::Policy::UCT::uct_t< HerderState > UCT;

	int _resources; // MpiBasicAttribute
	float _starvationDays; // MpiBasicAttribute
	// number of animals
	int _herdSize; // MpiBasicAttribute
	// daily resources needed by every animal
	int _resourcesPerAnimal; // MpiBasicAttribute

	DecisionModel * _model;
	BasePolicy * _uctBasePolicy;

	// mdp
	int _horizon; // MpiBasicAttribute
	int _width; // MpiBasicAttribute
	int _explorationBonus; // MpiBasicAttribute

	Village * _village;

	// mental map, quality of knowledge (years of the information)
	std::string _knowledgeMap;
	// mental map, value of resources
	std::string _resourcesMap;
public:
	// todo remove environment from here
	Herder( const std::string & id, int herdSize, int resourcesPerAnimal);
	virtual ~Herder();

	void configureMDP( const int & horizon, const int & width, const int & explorationBonus );
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

	void setResources( int resources );
	int getResources() const;
	// resources to consume every step in order to avoid death by starvation
	int getNeededResources() const;
	const Village & getVillage() const;
	void setVillage(Village * village);
	int getHerdSize() const;
	void createKnowledge();
	void updateKnowledge();

	const std::string & getKnowledgeMap() const;
	const std::string & getResourcesMap() const;

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	Herder( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace GujaratCellphones

#endif // __Herder_hxx__

