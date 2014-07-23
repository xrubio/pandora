
#ifndef __GujaratConfig_hxx__
#define __GujaratConfig_hxx__

#include <Config.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <Size.hxx>
#include <tinyxml.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace Gujarat
{

class GujaratWorld;

class GujaratConfig : public Engine::Config
{ 
    float _massToEnergyRate;
	float _energyToCalRate;

	// Agents social range expressed in # GIS data grid tiles
	int	_socialRange;
	// # days corresponding to a climate model season	
	int	_daysPerSeason;
	int	_daysPerYear;

	// # meters corresponding to the side of a cell
	float _cellResolution;
	// # real cells for each one in a map on low resolution
	int _lowResolution;

	// quantity of biomass in one dune cell on a mean rain year
	float _duneBiomass;
	// percentage of dune biomass that can be used for H/G
	float _duneEfficiency;
	// minimum percentage of dune biomass at the end of the year
	float _duneMinimum;
	// quantity of biomass in one interdune cell on a mean rain year
	float _interduneBiomass;
	// percentage of interdune biomass that can be used for H/G
	float _interduneEfficiency;
	// minimum percentage of interdune biomass at the end of the year
	float _interduneMinimum;

    std::string _biomassDistribution;
	std::string _distWaterFile;
	std::string _weightWaterFile;
	// constant equal to Numcells/sum of weights to water
	double _waterDistConstant;

	std::string _soilFile;
	std::string _demFile;

	int	_climateSeed;
	float _rainHistoricalDistribMean;
	float _rainHistoricalDistribStdev;

	int _numHG;
	// Agent home range expressed in # of underlying GIS data grid (~30m)
	int	_homeRange;
	int	_adulthoodAge;
	int	_numSectors;
	float _walkingSpeedHour;
	float _forageTimeCost;
	
    std::string	_hunterGathererController;

	// MRJ: Hunter Gatherers attributes
	int	_surplusForReproductionThreshold;
	int	_surplusWanted;	
	//float   _hgFoodNeedsForReproduction;
	//float   _hgSpoilageFoodFactor;	
	// available forage time for each age until adulthood
	std::vector<float> _availableForageTime;
	std::vector<int> _caloricRequirements;
	
	std::map< std::string, bool > _storeRasters;

	std::string	_demographicsModel;

	HunterGathererMDPConfig _controllerConfig;
public:
	GujaratConfig( const std::string & xmlFile );  
	virtual ~GujaratConfig();
    
	void loadParams();
	friend std::ostream & operator<<( std::ostream & stream, const GujaratConfig & config )
	{
		return stream << "Config(" << "path:" << config._resultsFile << "," << "size:" << config._size << "," << "steps:" << config._numSteps << " soil file: " << config._soilFile << ")";
	}  
	bool isStorageRequired( std::string key ) const
	{
		std::map<std::string,bool>::const_iterator it = _storeRasters.find( key );
		if ( it == _storeRasters.end() ) return false;
		return it->second;
	}
	
	friend class GujaratAgent;
	friend class GujaratWorld;
	friend class Climate;
};

} // namespace Gujarat

#endif // __GujaratConfig_hxx__

