
#ifndef __GujaratWorld_hxx__
#define __GujaratWorld_hxx__

#include <World.hxx>
#include <Climate.hxx>
#include <SettlementAreas.hxx>
#include <string>

namespace Gujarat
{
class GujaratConfig;
class SettlementAreas;

// id's depends on GIS data
enum Soils
{
	WATER = 1,
	INTERDUNE = 2,
	DUNE = 3
};

enum ResourceType
{
	WILD = 1,
	DOMESTICATED = 2,
	FALLOW = 3,
	SEASONALFALLOW = 4
};

class GujaratWorld : public Engine::World
{
	long int _agentKey;
	Climate _climate;
	const GujaratConfig & _config;
	// biomass produced by a cell each year
	std::vector<float>	_yearlyBiomass;
	// biomass increase for each day of rain season per cell
	std::vector<float> _dailyRainSeasonBiomassIncrease;
	// biomass decrease for each day of rain season per cell
	std::vector<float> _dailyDrySeasonBiomassDecrease;
	// rectangular Areas surrounding Dunes
	SettlementAreas    _settlementAreas;

	//*********************************************
	void createRasters();
	/*! \callgraph 
	* fill the world with agents
	*/
	void createAgents();

	void loadFile( const std::string & fileName, const std::string & rasterKey);

	void updateRainfall();
	void updateMoisture();
	void updateSoilCondition();
	void updateResources();

	void recomputeYearlyBiomass();
public:
	GujaratWorld( Engine::Simulation & simulation, const GujaratConfig & config );
	virtual ~GujaratWorld();
	
	// we need to redefine the step of the resource, as it will grow to max each step
	float moistureFunction(const Soils & soilType, const float & rain, const Seasons & season );
	void stepEnvironment();
	//void stepGeneralUpdate( int step );
	const Climate & getClimate() const;
	const GujaratConfig& getConfig() const { return _config; }

	int  convertToCalories( int mass );
	long int getNewKey();

	bool	isInterdune( Engine::Point2D<int> p );
	bool	isWild( Engine::Point2D<int> p );
	bool	isColdDrySeason();
	
	SettlementAreas * getSettlementAreas() { return & _settlementAreas; }
	const SettlementAreas* getSettlementAreas() const { return &_settlementAreas; }
};

} // namespace Gujarat


#endif // __GujaratWorld_hxx__

