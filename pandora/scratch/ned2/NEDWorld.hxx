
#ifndef __NEDWorld_hxx__
#define __NEDWorld_hxx__

#include "World.hxx"
#include "NEDConfig.hxx"
#include <string>

namespace NED
{
class Raster;
class Point2D;

class NEDWorld : public Engine::World
{
	int _numAgents;
	long int _agentKey;
	
	int _minHomeRange;
	int _maxHomeRange;
	int _minSplitChance;
	int _maxSplitChance;
	int _minCannibalism;
	int _maxCannibalism;
	
	std::string _resourceType;
	std::string _resourceFile;
	std::string _distrib;
	int         _peak;
	

	void createRasters();
	/*! \callgraph 
	* fill the world with agents
	*/
	void createAgents();

	// resource distribution
	void peakDistribution( int numOfPeaks );
	void createPeak( const Engine::Point2D<int> & seed );
	void randomUniformDistribution();
	void randomExponentialDistribution();
	void loadFile();

	// MPI
	MPI_Datatype * createTypeNeandertal();
	MPI_Datatype * createTypeSapiens();
	void registerTypes();
	void * createPackage( const std::string & type );
	Engine::Agent * createAgentFromPackage( const std::string & type, void * package );
public:
	NEDWorld( Engine::Simulation & simulation, const std::string & path, int numAgents, int minHomeRange, int maxHomeRange, int minSplitChance, int maxSplitChance, int minCannibalism, int maxCannibalism);
	NEDWorld( Engine::Simulation & simulation, NEDConfig & c);

	virtual ~NEDWorld();
	// we need to redefine the step of the resource, as it will grow to max each step
	void stepRaster( const std::string & key);
	void stepAgents();
	//void stepGeneralUpdate( int step );
	long int getNewKey();
};

} // namespace NED


#endif // __NEDWorld_hxx__

