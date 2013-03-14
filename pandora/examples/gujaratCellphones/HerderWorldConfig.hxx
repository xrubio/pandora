#ifndef __HerderWorldConfig_hxx__
#define __HerderWorldConfig_hxx__

#include <Config.hxx>

namespace GujaratCellphones
{

class HerderWorldConfig : public Engine::Config
{
	//environment
	int _size;
	int _averageResources;
	int _climateSeed;
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
	int _conversationsWetSeason;
	int _callsPerDayDrySeason;
	int _communications;
	int _cellphones;
	
public:
	HerderWorldConfig();
	virtual ~HerderWorldConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getSize() const;
	
	friend class HerderWorld;
	friend class Climate;
	friend class Herder;
};

} // namespace GujaratCellphones

#endif 
