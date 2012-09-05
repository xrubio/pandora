
#ifndef __GujaratConfig_hxx__
#define __GujaratConfig_hxx__

#include <Config.hxx>
#include <tinyxml.h>
#include <string>
#include <sstream>
#include <map>
#include <CaloricRequirementsTable.hxx>
#include <AgentInitializer.hxx>

namespace Gujarat
{

class GujaratWorld;
class HunterGathererMDPConfig;

class GujaratConfig : public Engine::Config
{ 
	int 		_size;		
	std::string 	_soilFile;
	std::string 	_demFile;
	std::string 	_duneMapFile;
	int		_climateSeed;
	// Agent home range expressed in # of underlying GIS
	// data grid (~30m)
	int		_homeRange;
	// Agents social range expressed in # GIS data grid tiles
	int		_socialRange;
	// # days corresponding to a climate model season	
	int		_daysPerSeason;
	int		_daysPerYear;
	// # meters corresponding to the side of a cell
	float _cellResolution;
	// # real cells for each one in a map on low resolution
	int _lowResolution;
	// MRJ: Hunter Gatherers attributes
	int		_surplusForReproductionThreshold;
	int		_surplusWanted;	
	float   _hgFoodNeedsForReproduction;
	//float   _hgSpoilageFoodFactor;	
	int		_adulthoodAge;
	float	_walkingSpeedHour;
	float	_forageTimeCost;
	float	_availableForageTime;
	

	// MRJ: Agro Pastoralists attributes
	int		_maxCropHomeDistance;

	int		_numSectors;
	float		_massToEnergyRate;
	float		_energyToCalRate;
	float		_duneBiomass;
	float		_duneBiomassStdDev;
	float		_duneEfficiency;
	float		_interduneBiomass;
	float		_interduneBiomassStdDev;
	float		_interduneEfficiency;

	float _rainHistoricalDistribShape;
	float _rainHistoricalDistribScale;
	float _rainHistoricalDistribMean;
	float _surplusSpoilage;

	std::string	_hunterGathererController;
	std::string	_demographicsModel;

	CaloricRequirementsTable* _hgCaloryRequirements;
	CaloricRequirementsTable* _apCaloryRequirements;
	AgentInitializer*	  _hgInitializer;
	AgentInitializer*	  _apInitializer;

	HunterGathererMDPConfig*	_controllerConfig;

	int _numHG;
	int _numAP;
	std::map< std::string, bool > _storeRasters;

	void 	parseSoilInfo(TiXmlElement * element);
	void	parseHGMDPConfig( TiXmlElement* element );
	
public:
	GujaratConfig();  
	virtual ~GujaratConfig();
    
	void extractParticularAttribs(TiXmlElement *pRoot);
	friend std::ostream & operator<<( std::ostream & stream, const GujaratConfig & config )
	{
		return stream << "Config(" << "path:" << config._resultsFile << "," << "size:" << config._size << "," << "steps:" << config._numSteps << " soil file: " << config._soilFile << ")";
	}  
	const int & getSize() const;
	bool	isStorageRequired( std::string key ) const
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

