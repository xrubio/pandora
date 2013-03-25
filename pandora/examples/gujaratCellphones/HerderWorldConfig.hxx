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
	bool _inVillageTransmission;
	// if values are -1 the herders of each village will share a common random uniform value
	int _inVillageTransmissionValue;
	bool _outVillageTransmission;
	int _outVillageTransmissionValue;
	
public:
	HerderWorldConfig();
	virtual ~HerderWorldConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getSize() const;
	
	friend class HerderWorld;
	friend class Climate;
	friend class Herder;
	friend class Village;
};

} // namespace GujaratCellphones

#endif 
