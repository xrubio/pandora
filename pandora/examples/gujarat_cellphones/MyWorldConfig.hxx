#ifndef __MyWorldConfig_hxx__
#define __MyWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class MyWorldConfig : public Engine::Config
{	
	int _cellphoneUsageMax;
	int _cellphoneUsageMin;
	int _daysDrySeason;
	int _daysWetSeason;
	int _diffVillageDaysNoCallsLow;
	int _diffVillageDaysNoCallsMedium;
	int _maxPercentMapSharedInACall;
	int _maxYearsCellInfo;
	int _multiplierCellsToAsk;
	int _numAgents;
	int _numAnimalsMax;
	int _numAnimalsMaxIni;
	int _numAnimalsMin;
	int _numAnimalsMinIni;
	int _numVillages;
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
	int _resourcesHighLevel;
	int _resourcesLowLevel;
	int _sameVillageDaysNoCallsHigh;
	int _sameVillageDaysNoCallsMedium;
	int _size;

public:
	MyWorldConfig();
	virtual ~MyWorldConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getCellphoneUsageMax() const;
	int getCellphoneUsageMin() const;
	int getDaysDrySeason() const;
	int getDaysWetSeason() const;
	int getDiffVillageDaysNoCallsLow() const;
	int getDiffVillageDaysNoCallsMedium() const;
	int getMaxPercentMapSharedInACall() const;
	int getMaxYearsCellInfo() const;
	int getMultiplierCellsToAsk() const;
	int getNumAgents() const;
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
	int getResourcesHighLevel() const;
	int getResourcesLowLevel() const;
	int getSameVillageDaysNoCallsHigh() const;
	int getSameVillageDaysNoCallsMedium() const;
	int getSize() const;

	friend class MyWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__
