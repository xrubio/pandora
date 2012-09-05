
#ifndef __SapiensGroup_hxx__
#define __SapiensGroup_hxx__

#include "Agent.hxx"
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

	int _age;
	int _splitChance;
	int _children;
} SapiensGroupPackage;


class SapiensGroup : public Engine::Agent
{
	//static const std::string _type;
	int _resources;
	int _population;
	int _homeRange;

	int _age;
	int _splitChance;
	int _children;

	void lookForResources();
	void collectResources();

	// if move, it can move to adjacent cells more suitable for hunting preys than actual one
	//SapiensGroup * lookForPrey( bool move );
	//void cannibalize( SapiensGroup * prey );
	//int countNeighbours( const Engine::Point2D<int> & position );

	void adjustPopulation();

	bool checkDeath();
	void checkSplit();
	//Engine::Point2D<int> getSplitLocation( int minimumResources );
	Engine::Point2D<int> getSplitLocation();

public:
	SapiensGroup( const std::string & id );
	SapiensGroup( const SapiensGroupPackage & package );
	virtual ~SapiensGroup();

	void serialize();
	void step();
	
	void * createPackage();

	void setHomeRange( const int & homeRange);
	void setSplitChance( const int & splitChance );

	void setPopulation( const int & population);
	const int & getPopulation();
};

} // namespace NED

#endif // __SapiensGroup_hxx__

