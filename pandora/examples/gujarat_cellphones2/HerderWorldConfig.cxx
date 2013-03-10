#include <HerderWorldConfig.hxx>

namespace GujaratCellphones
{

HerderWorldConfig::HerderWorldConfig() {
}

HerderWorldConfig::~HerderWorldConfig() {
}

void HerderWorldConfig::extractParticularAttribs(TiXmlElement * root) {
	TiXmlElement * element = root->FirstChildElement("numAgentsPerVillage");
	retrieveAttributeMandatory( element, "value", _numAgentsPerVillage);
	element = root->FirstChildElement("agentBirthResources");
	retrieveAttributeMandatory( element, "value", _agentBirthResources);
	element = root->FirstChildElement("agentNeededResources");
	retrieveAttributeMandatory( element, "value", _agentNeededResources);
	element = root->FirstChildElement("cellphoneUsageMax");
	retrieveAttributeMandatory( element, "value", _cellphoneUsageMax);
	element = root->FirstChildElement("cellphoneUsageMin");
	retrieveAttributeMandatory( element, "value", _cellphoneUsageMin);
	element = root->FirstChildElement("climateSeed");
	retrieveAttributeMandatory( element, "value", _climateSeed);
	element = root->FirstChildElement("daysDrySeason");
	retrieveAttributeMandatory( element, "value", _daysDrySeason);
	element = root->FirstChildElement("diffVillageDaysNoCallsLow");
	retrieveAttributeMandatory( element, "value", _diffVillageDaysNoCallsLow);
	element = root->FirstChildElement("diffVillageDaysNoCallsMedium");
	retrieveAttributeMandatory( element, "value", _diffVillageDaysNoCallsMedium);
	element = root->FirstChildElement("explorationBonus");
	retrieveAttributeMandatory( element, "value", _explorationBonus);
	element = root->FirstChildElement("horizon");
	retrieveAttributeMandatory( element, "value", _horizon);
	element = root->FirstChildElement("maxPercentMapSharedInACall");
	retrieveAttributeMandatory( element, "value", _maxPercentMapSharedInACall);
	element = root->FirstChildElement("maxYearsCellInfo");
	retrieveAttributeMandatory( element, "value", _maxYearsCellInfo);
	element = root->FirstChildElement("multiplierCellsToAsk");
	retrieveAttributeMandatory( element, "value", _multiplierCellsToAsk);
	element = root->FirstChildElement("numAnimalsMax");
	retrieveAttributeMandatory( element, "value", _numAnimalsMax);
	element = root->FirstChildElement("numAnimalsMaxIni");
	retrieveAttributeMandatory( element, "value", _numAnimalsMaxIni);
	element = root->FirstChildElement("numAnimalsMin");
	retrieveAttributeMandatory( element, "value", _numAnimalsMin);
	element = root->FirstChildElement("numAnimalsMinIni");
	retrieveAttributeMandatory( element, "value", _numAnimalsMinIni);		
	element = root->FirstChildElement("numVillages");
	retrieveAttributeMandatory( element, "value", _numVillages);
	element = root->FirstChildElement("rainHistoricalDistribution");
	retrieveAttributeMandatory(element, "mean", _rainHistoricalDistribMean);
	retrieveAttributeMandatory(element, "stdev", _rainHistoricalDistribStdDev);	
	element = root->FirstChildElement("percentAgentsWithCellphone");
	retrieveAttributeMandatory( element, "value", _percentAgentsWithCellphone);
	element = root->FirstChildElement("percentKnownAgentsDifferentVillageMediumAffinity");
	retrieveAttributeMandatory( element, "value", _percentKnownAgentsDifferentVillageMediumAffinity);
	element = root->FirstChildElement("percentKnownAgentsSameVillageHighAffinity");
	retrieveAttributeMandatory( element, "value", _percentKnownAgentsSameVillageHighAffinity);
	element = root->FirstChildElement("percentMapKnownAtBeginning");
	retrieveAttributeMandatory( element, "value", _percentMapKnownAtBeginning);
	element = root->FirstChildElement("probabilityCallHighAffinity");
	retrieveAttributeMandatory( element, "value", _probabilityCallHighAffinity);
	element = root->FirstChildElement("probabilityCallLowAffinity");
	retrieveAttributeMandatory( element, "value", _probabilityCallLowAffinity);
	element = root->FirstChildElement("probabilityCallMediumAffinity");
	retrieveAttributeMandatory( element, "value", _probabilityCallMediumAffinity);
	element = root->FirstChildElement("probabilityChildInheritsFromFather");
	retrieveAttributeMandatory( element, "value", _probabilityChildInheritsFromFather);
	element = root->FirstChildElement("probabilityExchangeInfoHighAffinityInVillage");
	retrieveAttributeMandatory( element, "value", _probabilityExchangeInfoHighAffinityInVillage);
	element = root->FirstChildElement("probabilityExchangeInfoMediumAffinityInVillage");
	retrieveAttributeMandatory( element, "value", _probabilityExchangeInfoMediumAffinityInVillage);
	element = root->FirstChildElement("probabilityKnowAgentDifferentVillageAtStart");
	retrieveAttributeMandatory( element, "value", _probabilityKnowAgentDifferentVillageAtStart);
	element = root->FirstChildElement("probabilityKnowAgentSameVillageAtStart");
	retrieveAttributeMandatory( element, "value", _probabilityKnowAgentSameVillageAtStart);
	element = root->FirstChildElement("probabilityMeetAgentSameCell");
	retrieveAttributeMandatory( element, "value", _probabilityMeetAgentSameCell);
	element = root->FirstChildElement("resourcesAgentCanGetPerDay");
	retrieveAttributeMandatory( element, "value", _resourcesAgentCanGetPerDay);
	element = root->FirstChildElement("resourcesHighLevel");
	retrieveAttributeMandatory( element, "value", _resourcesHighLevel);
	element = root->FirstChildElement("resourcesLowLevel");
	retrieveAttributeMandatory( element, "value", _resourcesLowLevel);
	element = root->FirstChildElement("animalsPerHerder");
	retrieveAttributeMandatory( element, "value", _animalsPerHerder);
	element = root->FirstChildElement("resourcesNeededPerAnimal");
	retrieveAttributeMandatory( element, "value", _resourcesNeededPerAnimal);
	element = root->FirstChildElement("resourcesNeededToGetANewAnimal");
	retrieveAttributeMandatory( element, "value", _resourcesNeededToGetANewAnimal);
	element = root->FirstChildElement("sameVillageDaysNoCallsHigh");
	retrieveAttributeMandatory( element, "value", _sameVillageDaysNoCallsHigh);
	element = root->FirstChildElement("sameVillageDaysNoCallsMedium");
	retrieveAttributeMandatory( element, "value", _sameVillageDaysNoCallsMedium);
	element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "value", _size);
	element = root->FirstChildElement("width");
	retrieveAttributeMandatory( element, "value", _width);
}

int HerderWorldConfig::getAgentBirthResources() const {
	return _agentBirthResources;
}

int HerderWorldConfig::getAgentNeededResources() const {
	return _agentNeededResources;
}

int HerderWorldConfig::getCellphoneUsageMax() const {
	return _cellphoneUsageMax;
}

int HerderWorldConfig::getCellphoneUsageMin() const {
	return _cellphoneUsageMin;
}

int HerderWorldConfig::getDaysDrySeason() const {
	return _daysDrySeason;
}

int HerderWorldConfig::getExplorationBonus() const {
	return _explorationBonus;
}

int HerderWorldConfig::getDiffVillageDaysNoCallsLow() const {
	return _diffVillageDaysNoCallsLow;
}

int HerderWorldConfig::getDiffVillageDaysNoCallsMedium() const {
	return _diffVillageDaysNoCallsMedium;
}
	
int HerderWorldConfig::getHorizon() const {
	return _horizon;
}

int HerderWorldConfig::getMaxPercentMapSharedInACall() const {
	return _maxPercentMapSharedInACall;
}

int HerderWorldConfig::getMaxYearsCellInfo() const {
	return _maxYearsCellInfo;
}

int HerderWorldConfig::getMultiplierCellsToAsk() const {
	return _multiplierCellsToAsk;
}

int HerderWorldConfig::getNumAnimalsMax() const {
	return _numAnimalsMax;
}

int HerderWorldConfig::getNumAnimalsMaxIni() const {
	return _numAnimalsMaxIni;
}

int HerderWorldConfig::getNumAnimalsMin() const {
	return _numAnimalsMin;
}

int HerderWorldConfig::getNumAnimalsMinIni() const {
	return _numAnimalsMinIni;
}

int HerderWorldConfig::getNumVillages() const {
	return _numVillages;
}

int HerderWorldConfig::getSize() const {
	return _size;
}

int HerderWorldConfig::getPercentAgentsWithCellphone() const {
	return _percentAgentsWithCellphone;
}

int HerderWorldConfig::getPercentKnownAgentsDifferentVillageMediumAffinity() const {
	return _percentKnownAgentsDifferentVillageMediumAffinity;
}

int HerderWorldConfig::getPercentKnownAgentsSameVillageHighAffinity() const {
	return _percentKnownAgentsSameVillageHighAffinity;
}

int HerderWorldConfig::getPercentMapKnownAtBeginning() const {
	return _percentMapKnownAtBeginning;
}

int HerderWorldConfig::getProbabilityCallHighAffinity() const {
	return _probabilityCallHighAffinity;
}

int HerderWorldConfig::getProbabilityCallLowAffinity() const {
	return _probabilityCallLowAffinity;
}

int HerderWorldConfig::getProbabilityCallMediumAffinity() const {
	return _probabilityCallMediumAffinity;
}

int HerderWorldConfig::getProbabilityChildInheritsFromFather() const {
	return _probabilityChildInheritsFromFather;
}

int HerderWorldConfig::getProbabilityExchangeInfoHighAffinityInVillage() const {
	return _probabilityExchangeInfoHighAffinityInVillage;
}

int HerderWorldConfig::getProbabilityExchangeInfoMediumAffinityInVillage() const {
	return _probabilityExchangeInfoMediumAffinityInVillage;
}

int HerderWorldConfig::getProbabilityKnowAgentDifferentVillageAtStart() const {
	return _probabilityKnowAgentDifferentVillageAtStart;
}

int HerderWorldConfig::getProbabilityKnowAgentSameVillageAtStart() const {
	return _probabilityKnowAgentSameVillageAtStart;
}

int HerderWorldConfig::getProbabilityMeetAgentSameCell() const {
	return _probabilityMeetAgentSameCell;
}

int HerderWorldConfig::getResourcesAgentCanGetPerDay() const {
	return _resourcesAgentCanGetPerDay;
}

int HerderWorldConfig::getResourcesHighLevel() const {
	return _resourcesHighLevel;
}

int HerderWorldConfig::getResourcesLowLevel() const {
	return _resourcesLowLevel;
}

int HerderWorldConfig::getResourcesNeededPerAnimal() const {
	return _resourcesNeededPerAnimal;
}

int HerderWorldConfig::getResourcesNeededToGetANewAnimal() const {
	return _resourcesNeededToGetANewAnimal;
}

int HerderWorldConfig::getSameVillageDaysNoCallsHigh() const {
	return _sameVillageDaysNoCallsHigh;
}

int HerderWorldConfig::getSameVillageDaysNoCallsMedium() const {
	return _sameVillageDaysNoCallsMedium;
}

int HerderWorldConfig::getWidth() const {
	return _width;
}
	
} // namespace GujaratCellphones
