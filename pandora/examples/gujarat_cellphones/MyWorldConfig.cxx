#include <MyWorldConfig.hxx>

namespace Examples
{

MyWorldConfig::MyWorldConfig() {
}

MyWorldConfig::~MyWorldConfig() {
}

void MyWorldConfig::extractParticularAttribs(TiXmlElement * root) {
	TiXmlElement * element = root->FirstChildElement("numAgents");
	retrieveAttributeMandatory( element, "value", _numAgents);
	element = root->FirstChildElement("diffVillageDaysNoCallsLow");
	retrieveAttributeMandatory( element, "value", _diffVillageDaysNoCallsLow);
	element = root->FirstChildElement("diffVillageDaysNoCallsMedium");
	retrieveAttributeMandatory( element, "value", _diffVillageDaysNoCallsMedium);
	element = root->FirstChildElement("maxYearsCellInfo");
	retrieveAttributeMandatory( element, "value", _maxYearsCellInfo);
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
	element = root->FirstChildElement("percentKnownAgentsDifferentVillageMediumAffinity");
	retrieveAttributeMandatory( element, "value", _percentKnownAgentsDifferentVillageMediumAffinity);
	element = root->FirstChildElement("percentKnownAgentsSameVillageHighAffinity");
	retrieveAttributeMandatory( element, "value", _percentKnownAgentsSameVillageHighAffinity);
	element = root->FirstChildElement("percentMapKnownAtBeginning");
	retrieveAttributeMandatory( element, "value", _percentMapKnownAtBeginning);
	element = root->FirstChildElement("probabilityChildInheritsFromFather");
	retrieveAttributeMandatory( element, "value", _probabilityChildInheritsFromFather);
	element = root->FirstChildElement("probabilityKnowAgentDifferentVillageAtStart");
	retrieveAttributeMandatory( element, "value", _probabilityKnowAgentDifferentVillageAtStart);
	element = root->FirstChildElement("probabilityKnowAgentSameVillageAtStart");
	retrieveAttributeMandatory( element, "value", _probabilityKnowAgentSameVillageAtStart);
	element = root->FirstChildElement("resourcesHighLevel");
	retrieveAttributeMandatory( element, "value", _resourcesHighLevel);
	element = root->FirstChildElement("resourcesLowLevel");
	retrieveAttributeMandatory( element, "value", _resourcesLowLevel);
	element = root->FirstChildElement("sameVillageDaysNoCallsHigh");
	retrieveAttributeMandatory( element, "value", _sameVillageDaysNoCallsHigh);
	element = root->FirstChildElement("sameVillageDaysNoCallsMedium");
	retrieveAttributeMandatory( element, "value", _sameVillageDaysNoCallsMedium);
	element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "value", _size);
}

int MyWorldConfig::getDiffVillageDaysNoCallsLow() const {
	return _diffVillageDaysNoCallsLow;
}

int MyWorldConfig::getDiffVillageDaysNoCallsMedium() const {
	return _diffVillageDaysNoCallsMedium;
}
	
int MyWorldConfig::getMaxYearsCellInfo() const {
	return _maxYearsCellInfo;
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

int MyWorldConfig::getPercentKnownAgentsDifferentVillageMediumAffinity() const {
	return _percentKnownAgentsDifferentVillageMediumAffinity;
}

int MyWorldConfig::getPercentKnownAgentsSameVillageHighAffinity() const {
	return _percentKnownAgentsSameVillageHighAffinity;
}

int MyWorldConfig::getPercentMapKnownAtBeginning() const {
	return _percentMapKnownAtBeginning;
}

int MyWorldConfig::getProbabilityChildInheritsFromFather() const {
	return _probabilityChildInheritsFromFather;
}

int MyWorldConfig::getProbabilityKnowAgentDifferentVillageAtStart() const {
	return _probabilityKnowAgentDifferentVillageAtStart;
}

int MyWorldConfig::getProbabilityKnowAgentSameVillageAtStart() const {
	return _probabilityKnowAgentSameVillageAtStart;
}

int MyWorldConfig::getResourcesHighLevel() const {
	return _resourcesHighLevel;
}

int MyWorldConfig::getResourcesLowLevel() const {
	return _resourcesLowLevel;
}

int MyWorldConfig::getSameVillageDaysNoCallsHigh() const {
	return _sameVillageDaysNoCallsHigh;
}

int MyWorldConfig::getSameVillageDaysNoCallsMedium() const {
	return _sameVillageDaysNoCallsMedium;
}

} // namespace Examples
