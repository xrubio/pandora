#ifndef __HerderWorldConfig_hxx__
#define __HerderWorldConfig_hxx__

#include <Config.hxx>

namespace GujaratCellphones
{

class HerderWorldConfig : public Engine::Config
{
	// climate
	int _climateSeed;
	float _rainHistoricalDistribMean;
	float _rainHistoricalDistribStdDev;
	int _daysDrySeason;

	// landscape
	int _size;

	// mdp
	int _explorationBonus;
	int _horizon;
	int _width;

	// agent demographics 
	int _numVillages;
	int _numAgentsPerVillage;
	int _animalsPerHerder;
	int _resourcesNeededPerAnimal;

	int _agentNeededResources;
	int _agentBirthResources;
	int _cellphoneUsageMax;
	int _cellphoneUsageMin;
	int _diffVillageDaysNoCallsLow;
	int _diffVillageDaysNoCallsMedium;


	int _maxPercentMapSharedInACall;
	int _maxYearsCellInfo;
	int _multiplierCellsToAsk;
	

	int _numAnimalsMax;
	int _numAnimalsMaxIni;
	int _numAnimalsMin;
	int _numAnimalsMinIni;
	int _percentAgentsWithCellphone;
	int _percentKnownAgentsDifferentVillageMediumAffinity;
	int _percentKnownAgentsSameVillageHighAffinity;
	int _percentMapKnownAtBeginning;
	int _probabilityCallHighAffinity;
	int _probabilityCallLowAffinity;
	int _probabilityCallMediumAffinity;
	int _probabilityChildInheritsFromFather;
	int _probabilityExchangeInfoHighAffinityInVillage;
	int _probabilityExchangeInfoMediumAffinityInVillage;
	int _probabilityKnowAgentDifferentVillageAtStart;
	int _probabilityKnowAgentSameVillageAtStart;
	int _probabilityMeetAgentSameCell;

	int _resourcesAgentCanGetPerDay;
	int _resourcesHighLevel;
	int _resourcesLowLevel;
	int _resourcesNeededToGetANewAnimal;
	int _sameVillageDaysNoCallsHigh;
	int _sameVillageDaysNoCallsMedium;

public:
	HerderWorldConfig();
	virtual ~HerderWorldConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getAgentBirthResources() const;
	int getAgentNeededResources() const;
	int getCellphoneUsageMax() const;
	int getCellphoneUsageMin() const;
	int getDiffVillageDaysNoCallsLow() const;
	int getDiffVillageDaysNoCallsMedium() const;
	int getExplorationBonus() const;
	int getHorizon() const;
	int getMaxPercentMapSharedInACall() const;
	int getMaxYearsCellInfo() const;
	int getMultiplierCellsToAsk() const;
	int getNumAnimalsMax() const;
	int getNumAnimalsMaxIni() const;
	int getNumAnimalsMin() const;
	int getNumAnimalsMinIni() const;
	int getNumVillages() const;
	int getPercentAgentsWithCellphone() const;
	int getPercentKnownAgentsDifferentVillageMediumAffinity() const;
	int getPercentKnownAgentsSameVillageHighAffinity() const;
	int getPercentMapKnownAtBeginning() const;
	int getProbabilityCallHighAffinity() const;
	int getProbabilityCallLowAffinity() const;
	int getProbabilityCallMediumAffinity() const;
	int getProbabilityChildInheritsFromFather() const;
	int getProbabilityExchangeInfoHighAffinityInVillage() const;
	int getProbabilityExchangeInfoMediumAffinityInVillage() const;
	int getProbabilityKnowAgentDifferentVillageAtStart() const;
	int getProbabilityKnowAgentSameVillageAtStart() const;
	int getProbabilityMeetAgentSameCell() const;
	float getRainHistoricalDistribMean() const;
	float getRainHistoricalDistribStdDev() const;
	int getResourcesAgentCanGetPerDay() const;
	int getResourcesHighLevel() const;
	int getResourcesLowLevel() const;
	int getResourcesNeededPerAnimal() const;
	int getResourcesNeededToGetANewAnimal() const;
	int getSameVillageDaysNoCallsHigh() const;
	int getSameVillageDaysNoCallsMedium() const;
	int getSize() const;
	int getWidth() const;

	int getDaysDrySeason() const;

	friend class HerderWorld;
	friend class Climate;
};

} // namespace GujaratCellphones

#endif 
