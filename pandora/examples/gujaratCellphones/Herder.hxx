#ifndef __Herder_hxx__
#define __Herder_hxx__

#include <Agent.hxx>
#include <Action.hxx>
#include <HerderWorld.hxx>
#include "DecisionModel.hxx"
#include "HerderState.hxx"
#include "HerderWorldConfig.hxx"
#include <engine/policy.h>
#include <engine/uct.h>

namespace GujaratCellphones
{

class Village;

class Herder : public Engine::Agent
{
	typedef Online::Policy::random_t< HerderState> BasePolicy;
	typedef Online::Policy::UCT::uct_t< HerderState > UCT;

	DecisionModel * _model;
	BasePolicy * _uctBasePolicy;

	//BasicAttributes
	int _resources; 
	float _starvationDays;
	int _herdSize; //number of animals
	int _resourcesPerAnimal; // daily resources needed by each animal
	Village * _village;
	HerderWorldConfig _config;
	HerderWorld * _world;

	// mdp
	int _horizon; // MpiBasicAttribute
	int _width; // MpiBasicAttribute
	int _explorationBonus; // MpiBasicAttribute

	//mental map
	std::string _knowledgeMap; //quality of knowledge (years of the information)
	std::string _resourcesMap; //value of resources


public:
	Herder( const std::string & id, int herdSize, int resourcesPerAnimal, const HerderWorldConfig config, HerderWorld* w, Village* v );
	virtual ~Herder();

	//sets and gets
	void setResources( int resources );
	int getResources() const;
	int getNeededResources() const;
	void setVillage( Village * village );
	const Village & getVillage() const;
	int getHerdSize() const;
	const std::string & getKnowledgeMap() const;
	const std::string & getResourcesMap() const;

	//mental map
	void createKnowledge();
	void updateKnowledge();

	//interactions
	void talkToOtherShepherds( int numberOfShepherds );
	//void printRaster(Engine::Raster & raster);
	void exchangeInformationWithOtherShepherd( const std::string & idShepherdReceivesCall );

	//mdp
	void configureMDP( const int & horizon, const int & width, const int & explorationBonus );
	void selectActions();
	void updateState();

	//register information
	void registerAttributes();
	void serialize();


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

