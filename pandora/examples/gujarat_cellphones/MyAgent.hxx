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
	void initMadeCalls();
	void initCellsSharedPerCall();
	void initLastCalls();
	void updateNumberOfAnimals();
	void updateMentalWorldRepresentation();
	void updateMentalWorldRepresentationAccuracy();
	void resetNumberOfResourcesGathered();
	void gatherResources();
	void move();
	bool hasMinimumNumOfAnimals();
	void stopBeingAShepherd();
	void checkConditions();
	void fission();
	bool agentFissions();
	void updateReputation();
	void updateAvgSharedCellsPerCall(int sharedCells);
	void updateAffinities();

	std::vector<Engine::Point2D<int> > getUnknownNeighborCells(int x, int y);
	
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
	std::vector<std::pair<std::string,int> > _socialNetwork; //first is an id of an agent, second is the affinity
	double _mentalWorldRepresentationAccuracy;
	Examples::MyWorldConfig _config;
	MyWorld* _world;
	int _reputation;
	int _madeCalls;
	double _avgCellsSharedPerCall;
	std::vector<std::pair<std::string, int> > _lastCalls; //first is and id of an agent, second is days since last call was made

	void setVillage(MyVillage &v);
	void initPosition();
	void setGatheredResources(int resources);
	void setNumberOfAnimals(int animals);
	void setHasCellphone(bool cellphone);
	void setCooperationTreat(int cooperationTreat);
	void setCellphoneUsage(int cellphoneUsage);
	double getAvgCellsSharedPerCall();
	void deleteContactInSocialNetwork(std::string id);
	void updateCellMentalWorldRepresentation(int x, int y, int resourcesLevel, int time);
	void callMade(std::string id);
	void updateLastCalls();
	void createAffinity(std::string id, int affinityLevel);
	MyVillage getVillage();
	void deleteAffinity(std::string id);

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

