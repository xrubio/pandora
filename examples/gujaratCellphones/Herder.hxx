#ifndef __Herder_hxx__
#define __Herder_hxx__

#include <Agent.hxx>
#include <engine/policy.h>
#include <engine/uct.h>
#include "HerderState.hxx"
#include "Village.hxx"

namespace GujaratCellphones
{

class DecisionModel;

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

	// mdp
	int _horizon; // MpiBasicAttribute
	int _width; // MpiBasicAttribute
	int _explorationBonus; // MpiBasicAttribute

	//mental map
	std::string _knowledgeMap; //quality of knowledge (years of the information)
	std::string _resourcesMap; //value of resources

	// knowledge transmission
	void inVillageKnowledgeTransmission() const;
	void outVillageKnowledgeTransmission() const;
	void knowledgeTransmission( int frequency ) const;
	void copyValue( const Herder & origin, const Herder & target, const Engine::Point2D<int> & index ) const;
	void shareCell( const Herder & herderA, const Herder & herderB, const Engine::Point2D<int> & index ) const;
		
public:
	Herder( const std::string & id, int herdSize, int resourcesPerAnimal, Village & village );
	virtual ~Herder();

	//sets and gets
	void setResources( int resources );
	int getResources() const;
	int getNeededResources() const;
	int getHerdSize() const;
	void setHerdSize( int herdSize );
	const std::string & getKnowledgeMap() const;
	const std::string & getResourcesMap() const;
	const Village & getVillage() const;

	//mental map
	void createKnowledge();
	void updateKnowledge();
	void shareKnowledge( Herder & herder ) const;

	//void printRaster(Engine::DynamicRaster & raster);

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
	Herder( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace GujaratCellphones

#endif // __Herder_hxx__

