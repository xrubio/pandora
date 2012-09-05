
#ifndef __NeandertalGroup_hxx__
#define __NeandertalGroup_hxx__

#include "Agent.hxx"
#include "World.hxx"
#include <string>

#include "Point2D.hxx"

namespace NED
{

typedef struct
{
	char _id[32];
	Engine::Point2D<int> _position;	
	int _resources;
	bool _exists;

	int _population;
	int _homeRange;
	int _cannibalism;

	int _stepsUsingCannibalism;
	int _age;
	int _splitChance;
	int _children;
	
	int _minForkResources;
	int _splitWaste;
	int _maxAge;
	int _minFoodToEat;
	int _minFoodToMove;

} NeandertalGroupPackage;

class NeandertalGroup : public Engine::Agent
{
	// state
	int _resources;
	int _population;
	int _homeRange;
	int _cannibalism;

	int _stepsUsingCannibalism;
	int _age;
	int _splitChance;
	int _children;

	// params
	int _minForkResources;
	int _splitWaste;
	int _maxAge;
	int _minFoodToEat;
	int _minFoodToMove;

	// methods
	void lookForResources();
	void collectResources();

	void lookForPreys( Engine::World::AgentsList & preys );
	void cannibalize( NeandertalGroup * prey );

	void adjustPopulation();

	bool checkDeath();
	void checkSplit();
	//Engine::Point2D<int> getSplitLocation( int minimumResources );
	Engine::Point2D<int> getSplitLocation();

public:
	NeandertalGroup( const std::string & id );

	NeandertalGroup( const NeandertalGroupPackage & package );
	
	NeandertalGroup(  const std::string & id
			, Engine::Point2D<int> position
			, bool exists
			, Engine::World* world
			, int resources
			, int population
			, int homeRange
			, int cannibalism
			, int stepsUsingCannibalism
			, int age
			, int splitChance
			, int children
			, int minForkResources
			, int splitWaste
			, int maxAge
			, int minFoodToEat
			, int minFoodToMove);

	virtual ~NeandertalGroup();

	
	void serialize();
	void step();
	void offspringAtStep();
	void death();
	void eat();
	void consumption();
	bool starvation();
	/*
	friend std::ostream & operator<<( std::ostream & stream, NeandertalGroup & agent );
	friend std::ostream & operator<<( std::ostream & stream, NeandertalGroup * agent );
	*/

	void * createPackage();

	void setHomeRange( const int & homeRange );
	void setSplitChance( const int & spliChance );
	void setCannibalism( const int & cannibalism );
	void setResources( const int & resources );

	const int & getPopulation();
	void setPopulation( const int & population );
};

} // namespace NED

#endif // __NeandertalGroup_hxx__

