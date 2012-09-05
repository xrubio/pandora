
#ifndef __SugarWorld_hxx__
#define __SugarWorld_hxx__

#include "World.hxx"
#include "SugarConfig.hxx"
#include <string>

namespace SUG
{
class Raster;
class Point2D;

enum SeasonCategories { SUMMER, WINTER };


class SugarWorld : public Engine::World
{
	float _alpha;
	SeasonCategories _seasonNorth;
	SeasonCategories _seasonSouth;

//
	int _numAgents;
	long int _agentKey;	
	
	std::string _resourceType;
	std::string _resourceFile;

	void createRasters();
	/*! \callgraph 
	* fill the world with agents
	*/
	void createAgents();

	
	// MPI
	MPI_Datatype * createSugarAgent();
	void registerTypes();
	void * createPackage( const std::string & type );
	Engine::Agent * createAgentFromPackage( const std::string & type, void * package );	
	MPI_Datatype * createTypeSugarAgent();
public:
	SugarConfig _config;
	
	SugarWorld( Engine::Simulation & simulation, SugarConfig & c);

	virtual ~SugarWorld();
	
	// stepRaster = sugarscapeGrowback
	void stepRaster( const std::string & key);
	void stepEnvironment( const std::string & key);
	void growBack();
	void pollutionDiffusion();
	//void stepAgents();
	int reserveAgentKey() { return _agentKey++;}
	//void stepGeneralUpdate( int step );
	long int getNewKey();
	void replace();

};

} // namespace SUG


#endif // __SugarWorld_hxx__

