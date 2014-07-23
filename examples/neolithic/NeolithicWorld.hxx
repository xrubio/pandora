
#ifndef __NeolithicWorld_hxx
#define __NeolithicWorld_hxx

#include <World.hxx>

namespace Examples 
{

enum Rasters
{
	eDem,
	eInitPopulation,
	ePopulation,
	ePopulationBase,
	eArrivalTime
};

class NeolithicConfig;

class NeolithicWorld : public Engine::World
{
	void createRasters();

	void stepEnvironment();

	bool isMountain( const Engine::Point2D<int> & index );
	bool isSea( const Engine::Point2D<int> & index );
	void disperseTo( const Engine::Point2D<int> & origin, const Engine::Point2D<int> & destination, const int & migrationPopulation );

	void reproductionStep();
	void arrivalCheck();

	void homogeneousDispersionStep();
	void nonHomogeneousDispersionStep();

	void travelBySea( const Engine::Point2D<int> & origin, const Engine::Point2D<int> & destination, const int & migrationPopulation );	
	int countNeighbors( const Engine::Point2D<int> & index );

public:
    NeolithicWorld(NeolithicConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~NeolithicWorld();
};

} // namespace Examples 


#endif // __NeolithicWorld_hxx

