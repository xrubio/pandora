
#ifndef __Earth_hxx__
#define __Earth_hxx__

#include <World.hxx>

namespace Examples 
{

enum Rasters
{
    eDem,
    ePopulation,
    eHumans,
    eNewCases,
    eZombies
};

class EarthConfig;

class Earth : public Engine::World
{
	void createRasters();
	void createAgents();

	std::list<Engine::Point2D<int> > _newCases;
	void stepEnvironment();
public:
	Earth( EarthConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~Earth();

	float getZombieVirulence() const;
	void addZombie( const Engine::Point2D<int> & position );
};

} // namespace Examples 

#endif // __Earth_hxx__

