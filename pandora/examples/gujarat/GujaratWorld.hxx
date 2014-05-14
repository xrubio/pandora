
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
	eDistWater,
	eWeightWater,
	eDuneMap,
	eResources,
	eResourcesFraction,
	eForageActivity,
	eHomeActivity,
	eFarmingActivity,
	eResourceType,
	eConsecutiveYears,
	eSectors,
	eMoisture,
	// LOW RESSOLUTION
	eLRResources,
	eLRResourcesFraction,
	eLRForageActivity,
	eLRHomeActivity,
	eLRFarmingActivity,
	eLRMoisture,
	eLRWeightWater,
	eLRPopulation,
	LRCounterSoilDUNE,
	LRCounterSoilINTERDUNE,
	LRCounterSoilWATER,
	
	
	eSectorUtility,
	paintLRResources
	
	,eCounterHRSectors
	,eCounterLRSectors
};

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
	// biomass of a cell at the end of the previous year
	std::vector<float> _remainingBiomass;

	//****?
	//std::vector<int> vFOO;
//	int * _vFOO;
	
	//*********************************************
	void createRasters();
	/*! \callgraph 
	* fill the world with agents
	*/
	void createAgents();

	void loadFile( const std::string & fileName, const std::string & rasterKey);

	void updateRainfall();
	//void updateMoisture();
	void updateSoilCondition();
	void updateResources();
	void updateResourcesFOO();
	void updateResourcesLowResMap();
	void recomputeYearlyBiomass();
	void recomputeLowResYearlyBiomass();
	
	//Engine::Point2D<int> findNearestWater( const Engine::Point2D<int> & point );
	float getBiomassVariation( bool wetSeason, Soils & cellSoil, const Engine::Point2D<int> & index ) const;
public:
	GujaratWorld( Engine::Simulation & simulation, const GujaratConfig & config );
	virtual ~GujaratWorld();
	
	void stepEnvironment();
	//void stepGeneralUpdate( int step );
	const Climate & getClimate() const;
	const GujaratConfig& getConfig() const { return _config; }

	int  convertToCalories( int mass );
	long int getNewKey();

	SettlementAreas * getSettlementAreas() { return & _settlementAreas; }
	const SettlementAreas* getSettlementAreas() const { return &_settlementAreas; }
	
	
	void worldCell2LowResCell( Engine::Point2D<int> pos, Engine::Point2D<int> & result ) const;
	void LowRes2HighResCellCorner(Engine::Point2D<int> pos, Engine::Point2D<int> &result ) const;
	
	int getLowResMapsSideSize();
	
	void fillLRRaster(enum Rasters idLRRaster, int val);
	void fillRaster(enum Rasters iRaster, int val);
	
	void fillLowResCounterRaster(enum Rasters idRasterCounter, enum Rasters idRasterSource,int soiltype);
	void LowResRasterCountsHighResRaster(enum Rasters idRasterCounter, enum Rasters idRasterSource);
	void fillLowResMeanRaster(enum Rasters idRasterCounter, enum Rasters idRasterSource);
	
	//! returns the value of raster r in global position "position"
	int getValueGW( const Engine::Raster & r, const Engine::Point2D<int> & position ) const;
	
	//! returns the value of raster "index" in global position "position"
	int getValueLR( const int & index, const Engine::Point2D<int> & position ) const;
	//! sets the value of raster "index" to value "value" in global position "position"
	void setValueLR( const int & index, const Engine::Point2D<int> & position, int value );
	//! sets the init value of raster "index" to value "value" in global position "position"
	void setInitValueLR( const int & index, const Engine::Point2D<int> & position, int value );
	
	//! returns the value of raster "r" in global position "position"
	int getValueLR( const Engine::Raster & r, const Engine::Point2D<int> & position ) const;
	//! sets the value of raster "r" to value "value" in global position "position"
	void setValueLR( Engine::Raster & r, const Engine::Point2D<int> & position, int value );
	//! sets the init value of raster "r" to value "value" in global position "position"
	void setInitValueLR( Engine::Raster & r, const Engine::Point2D<int> & position, int value );
	
	void ExpandLR2HRRaster(const int LRSource, const int HRTarget)
	{
		Engine::Point2D<int> index;
		
		for(index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++)		
		{	
			for(index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++)		
			{				
				Engine::Point2D<int> mapCell; 
				worldCell2LowResCell(index, mapCell);
				int v = getValueLR(LRSource,mapCell);
				setValue(HRTarget,index,v);
			}
		}
	}
	
	void getHRFreeCell(const Engine::Point2D<int> LRpos, Engine::Point2D<int> & HRpos);
	
	
//*****************************************************************
//*****************************************************************
//*****************************************************************
	Engine::Point2D<int> _firstHome;
	
	void fillWithValue(Engine::Raster & r, int val);
	
	void fillWithValue(enum Rasters r, int val);
		
	void initDebugRasters();
	
	void updateeSectorUtility(const std::vector< Engine::Point2D<int> > & cells, int val);
		
	void assignResourceFromSectorsToHR(const std::vector< Engine::Point2D<int> > & cells,int HRValToForce);
	
	
	void fillHRRasterWithLRRaster(enum Rasters idLRSource, enum Rasters idHRTarget);
	
	void fillIniRaster(enum Rasters idRaster, int val);
	
	void duneizeMap(enum Rasters idRaster, int bandWidth);
//****************************************
	
};

} // namespace Gujarat


#endif // __GujaratWorld_hxx__

