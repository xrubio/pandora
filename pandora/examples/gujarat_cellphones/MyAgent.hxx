#ifndef __MyAgent_hxx__
#define __MyAgent_hxx__

#include <Agent.hxx>
#include "MyWorld.hxx"
#include "MyWorldConfig.hxx"
#include "MyVillage.hxx"
#include <string>
#include <map>
#include <utility>

namespace Tutorial
{

class MyAgent : public Engine::Agent
{

protected:
	bool agentFissions();
	void checkConditions();
	void gatherResources();
	std::vector<Engine::Point2D<int> > getCellsToAskInVillage(const std::vector<std::vector<int> > &m, int numberOfCells);
	std::vector<Engine::Point2D<int> > getCellsToAskOutsideVillage(int numberOfCells);
	std::vector<std::vector<int> > getMatrixKnownNeighborCells();
	std::vector<Engine::Point2D<int> > getUnknownNeighborCells(int x, int y);
	void initCellphoneUsage();
	void initCellsSharedPerCall();
	void initCooperationTreat();
	void initHasCellphone();
	void initLastCalls();
	void initMadeCalls();
	void initMentalWorldRepresentation();
	void initNumberOfAnimals();
	void initReputation();
	void initSocialNetwork();
	void initWorld(MyWorld* w);
	int longestPathKnownCellsFromCell(int i, int j);
	
public:
	double _avgCellsSharedPerCall;
	int _cellphoneUsage;
	Examples::MyWorldConfig _config;
	int _cooperationTreat;
	int _gatheredResources;
	bool _hasCellphone;
	std::map<std::string, int> _lastCalls; //first is and id of an agent, second is days since last call was made
	int _madeCalls;
	std::pair<int, int> _mentalWorldRepresentation[60][60]; //first is info about the area, second is how old the info is
	int _numberOfAnimals;
	int _reputation;
	std::vector<std::pair<std::string,int> > _socialNetwork; //first is an id of an agent, second is the affinity
	std::vector<std::string> _spokeInVillage;	
	MyVillage _village;
	MyWorld* _world;

	MyAgent(const std::string &id, const Examples::MyWorldConfig &config, MyWorld* w, bool initialAgent);
	virtual ~MyAgent();
	void addSpokeInVillage(std::string id);
	bool agentIsInHisVillage();
	void callMade(std::string id);
	std::string chooseWhoToCall();
	void createAffinity(std::string id, int affinityLevel);
	bool decideToMakeACall();
	void deleteAffinity(std::string id);
	void exchangeInformationWithOtherAgent(std::string idAgentReceivesCall);
	void exchangeInfoWithPeopleInVillage();
	void fission();	
	int getAffinityWithAgent(std::string id);
	double getAvgCellsSharedPerCall();
	int getCellphoneUsage();
	int getCooperationTreat();
	int getGatheredResources();
	bool getHasCellphone();
	int getLastCall(std::string id);
	std::map<std::string, int>  getLastCalls();
	int getMadeCalls();
	int getNumberOfAnimals();
	int getReputation();
	std::vector<std::pair<std::string,int> > getSocialNetwork();
	int getTimeCellMentalWorldRepresentation(int x, int y);
	int getValueCellMentalWorldRepresentation(int x, int y);
	std::string getId();
	MyVillage getVillage();
	bool hasAffinityWithAgent(std::string id);
	bool hasCellphone();
	bool hasMinimumNumOfAnimals();
	void initPosition();
	bool knowsCell(int i, int j);
	void meetAgentsInSameCell();
	void move();
	int numberOfCellsWillingToTell(std::string idReceivingAgent);
	void registerAttributes();
	void resetSpokeInVillage();
	void resetNumberOfResourcesGathered();
	void returnToVillage();
	void serialize();
	void setCellphoneUsage(int cellphoneUsage);
	void setCooperationTreat(int cooperationTreat);
	void setGatheredResources(int resources);
	void setHasCellphone(bool cellphone);
	void setLastCall(std::string id, int days);
	void setNumberOfAnimals(int animals);
	void setVillage(MyVillage &v);
	void stopBeingAShepherd();
	void updateAffinities();
	void updateAvgSharedCellsPerCall(int sharedCells);
	void updateCellMentalWorldRepresentation(int x, int y, int resourcesLevel, int time);
	void updateMentalWorldRepresentation();
	void updateNumberOfAnimals();
	void updateLastCalls();
	void updateReputation();
	void updateYearsMentalWorldRepresentation();
	void updateState();

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

