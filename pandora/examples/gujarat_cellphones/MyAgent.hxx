#ifndef __MyAgent_hxx__
#define __MyAgent_hxx__

#include <Agent.hxx>
#include "MyWorld.hxx"
#include "MyWorldConfig.hxx"
#include "MyVillage.hxx"
#include <string>
#include <utility>

namespace Tutorial
{

class MyAgent : public Engine::Agent
{

protected:
	void initNumberOfAnimals();
	void initHasCellphone();
	void initMentalWorldRepresentation();
	void initCooperationTreat();
	void initCellphoneUsage();
	void initSocialNetwork();
	void initWorld(MyWorld* w);
	void initReputation();
	void initCallsMade();
	void initCellsSharedPerCall();
	void updateNumberOfAnimals();
	void updateMentalWorldRepresentation();
	void updateMentalWorldRepresentationAccuracy();
	void resetNumberOfResourcesGathered();
	void gatherResources();
	void move();
	bool hasMinimumNumberOfAnimals();
	void checkConditions();
	void fission();
	void fussion();
	bool agentFissions();
	bool agentFussions();
	void updateReputation();
	void updateAvgSharedCellsPerCall(int sharedCells);
	
public:
	MyAgent(const std::string &id, const Examples::MyWorldConfig &config, MyWorld* w);
	virtual ~MyAgent();

	void updateState();
	void serialize();
	void registerAttributes();

	int _gatheredResources;
	int _numberOfAnimals;
	bool _hasCellphone;
	std::pair<int, int> _mentalWorldRepresentation[60][60]; //first is info about the area, second is how old the info is
	int _cooperationTreat;
	int _cellphoneUsage;
	MyVillage _village;
	std::vector<std::pair<int,int> > _socialNetwork; //first is an id of an agent, second is the affinity
	double _mentalWorldRepresentationAccuracy;
	Examples::MyWorldConfig _config;
	MyWorld* _world;
	int _reputation;
	int _callsMade;
	double _avgCellsSharedPerCall;

	void setVillage(MyVillage &v);
	void initPosition();
	void setGatheredResources(int resources);
	void setNumberOfAnimals(int animals);
	void setHasCellphone(bool cellphone);
	void setCooperationTreat(int cooperationTreat);
	void setCellphoneUsage(int cellphoneUsage);
	double getAvgCellsSharedPerCall();

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

