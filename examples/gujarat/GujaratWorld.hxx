
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

enum Rasters
{
	eSoils,
	eDem,
	eResources,
	eForageActivity,
	eHomeActivity,
	eSectors,
	eDistWater,
	eWeightWater,
	eResourcesFraction
};

// id's depends on GIS data
enum Soils
{
	WATER = 1,
	INTERDUNE = 2,
	DUNE = 3
};

class GujaratWorld : public Engine::World
{

	long int _agentKey;
	Climate _climate;
	// biomass produced by a cell each year
	std::vector<float>	_yearlyBiomass;
	// biomass increase for each day of rain season per cell
	std::vector<float> _dailyRainSeasonBiomassIncrease;
	// biomass decrease for each day of rain season per cell
	std::vector<float> _dailyDrySeasonBiomassDecrease;
	// rectangular Areas surrounding Dunes
	SettlementAreas    _settlementAreas;
	// biomass of a cell at the end of the previous year
	std::vector<float> _remainingBiomass;

	//*********************************************
	void createRasters();
	/*! \callgraph 
	* fill the world with agents
	*/
	void createAgents();

	void loadFile( const std::string & fileName, const std::string & rasterKey);

	void updateRainfall();
	void updateResources();

	void recomputeYearlyBiomass();

	//Engine::Point2D<int> findNearestWater( const Engine::Point2D<int> & point );
	float getBiomassVariation( bool wetSeason, Soils & cellSoil, const Engine::Point2D<int> & index ) const;
public:
	GujaratWorld( GujaratConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~GujaratWorld();
	
	void stepEnvironment();
	//void stepGeneralUpdate( int step );
	const Climate & getClimate() const;

	int  convertToCalories( int mass );
	long int getNewKey();

	SettlementAreas * getSettlementAreas() { return & _settlementAreas; }
	const SettlementAreas* getSettlementAreas() const { return &_settlementAreas; }
};

} // namespace Gujarat


#endif // __GujaratWorld_hxx__

