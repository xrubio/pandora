#ifndef __HerderWorldConfig_hxx__
#define __HerderWorldConfig_hxx__

#include <Config.hxx>
#include <Size.hxx>

namespace GujaratCellphones
{

enum ResourceDistribution
{
    eRandom = 0,
    eIncrease = 1,
    eDistance = 2
};

class HerderWorldConfig : public Engine::Config
{
	//environment
    ResourceDistribution _randomDistribution;
	int _averageResources;
	int _daysDrySeason;
	float _rainHistoricalDistribMean;
	float _rainHistoricalDistribStdDev;

	//population
	int _numVillages;
	int _numAgentsPerVillage;
	int _animalsPerHerder;
	int _resourcesNeededPerAnimal;

	//mdp
	int _horizon;
	int _width;
	int _explorationBonus;

	//knowledge transmission
	bool _inVillageTransmission;
	// if values are -1 the herders of each village will share a common random uniform value
	int _inVillageTransmissionValue;
	bool _outVillageTransmission;
	int _outVillageTransmissionValue;
	
public:
	HerderWorldConfig( const std::string & xmlFile );
	virtual ~HerderWorldConfig();
    void loadParams();

	friend class HerderWorld;
	friend class Climate;
	friend class Herder;
	friend class Village;
};

} // namespace GujaratCellphones

#endif 
