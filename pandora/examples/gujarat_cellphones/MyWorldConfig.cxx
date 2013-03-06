#include <MyWorldConfig.hxx>

namespace GujaratCellphones
{

MyWorldConfig::MyWorldConfig() {
}

MyWorldConfig::~MyWorldConfig() {
}

void MyWorldConfig::extractParticularAttribs(TiXmlElement * root) {
	TiXmlElement * element = root->FirstChildElement("numAgents");
	retrieveAttributeMandatory( element, "value", _numAgents);
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

int MyWorldConfig::getAgentBirthResources() const {
	return _agentBirthResources;
}

int MyWorldConfig::getAgentNeededResources() const {
	return _agentNeededResources;
}

int MyWorldConfig::getCellphoneUsageMax() const {
	return _cellphoneUsageMax;
}

int MyWorldConfig::getCellphoneUsageMin() const {
	return _cellphoneUsageMin;
}

int MyWorldConfig::getDaysDrySeason() const {
	return _daysDrySeason;
}

int MyWorldConfig::getExplorationBonus() const {
	return _explorationBonus;
}

int MyWorldConfig::getDiffVillageDaysNoCallsLow() const {
	return _diffVillageDaysNoCallsLow;
}

int MyWorldConfig::getDiffVillageDaysNoCallsMedium() const {
	return _diffVillageDaysNoCallsMedium;
}
	
int MyWorldConfig::getHorizon() const {
	return _horizon;
}

int MyWorldConfig::getMaxPercentMapSharedInACall() const {
	return _maxPercentMapSharedInACall;
}

int MyWorldConfig::getMaxYearsCellInfo() const {
	return _maxYearsCellInfo;
}

int MyWorldConfig::getMultiplierCellsToAsk() const {
	return _multiplierCellsToAsk;
}

int MyWorldConfig::getNumAgents() const {
	return _numAgents;
}

int MyWorldConfig::getNumAnimalsMax() const {
	return _numAnimalsMax;
}

int MyWorldConfig::getNumAnimalsMaxIni() const {
	return _numAnimalsMaxIni;
}

int MyWorldConfig::getNumAnimalsMin() const {
	return _numAnimalsMin;
}

int MyWorldConfig::getNumAnimalsMinIni() const {
	return _numAnimalsMinIni;
}

int MyWorldConfig::getNumVillages() const {
	return _numVillages;
}

int MyWorldConfig::getSize() const {
	return _size;
}

int MyWorldConfig::getPercentAgentsWithCellphone() const {
	return _percentAgentsWithCellphone;
}

int MyWorldConfig::getPercentKnownAgentsDifferentVillageMediumAffinity() const {
	return _percentKnownAgentsDifferentVillageMediumAffinity;
}

int MyWorldConfig::getPercentKnownAgentsSameVillageHighAffinity() const {
	return _percentKnownAgentsSameVillageHighAffinity;
}

int MyWorldConfig::getPercentMapKnownAtBeginning() const {
	return _percentMapKnownAtBeginning;
}

int MyWorldConfig::getProbabilityCallHighAffinity() const {
	return _probabilityCallHighAffinity;
}

int MyWorldConfig::getProbabilityCallLowAffinity() const {
	return _probabilityCallLowAffinity;
}

int MyWorldConfig::getProbabilityCallMediumAffinity() const {
	return _probabilityCallMediumAffinity;
}

int MyWorldConfig::getProbabilityChildInheritsFromFather() const {
	return _probabilityChildInheritsFromFather;
}

int MyWorldConfig::getProbabilityExchangeInfoHighAffinityInVillage() const {
	return _probabilityExchangeInfoHighAffinityInVillage;
}

int MyWorldConfig::getProbabilityExchangeInfoMediumAffinityInVillage() const {
	return _probabilityExchangeInfoMediumAffinityInVillage;
}

int MyWorldConfig::getProbabilityKnowAgentDifferentVillageAtStart() const {
	return _probabilityKnowAgentDifferentVillageAtStart;
}

int MyWorldConfig::getProbabilityKnowAgentSameVillageAtStart() const {
	return _probabilityKnowAgentSameVillageAtStart;
}

int MyWorldConfig::getProbabilityMeetAgentSameCell() const {
	return _probabilityMeetAgentSameCell;
}

int MyWorldConfig::getResourcesAgentCanGetPerDay() const {
	return _resourcesAgentCanGetPerDay;
}

int MyWorldConfig::getResourcesHighLevel() const {
	return _resourcesHighLevel;
}

int MyWorldConfig::getResourcesLowLevel() const {
	return _resourcesLowLevel;
}

int MyWorldConfig::getResourcesNeededPerAnimal() const {
	return _resourcesNeededPerAnimal;
}

int MyWorldConfig::getResourcesNeededToGetANewAnimal() const {
	return _resourcesNeededToGetANewAnimal;
}

int MyWorldConfig::getSameVillageDaysNoCallsHigh() const {
	return _sameVillageDaysNoCallsHigh;
}

int MyWorldConfig::getSameVillageDaysNoCallsMedium() const {
	return _sameVillageDaysNoCallsMedium;
}

int MyWorldConfig::getWidth() const {
	return _width;
}
	
} // namespace GujaratCellphones
