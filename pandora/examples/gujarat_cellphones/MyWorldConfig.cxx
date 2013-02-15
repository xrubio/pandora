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
	element = root->FirstChildElement("probabilityChildInheritsFromFather");
	retrieveAttributeMandatory( element, "value", _probabilityChildInheritsFromFather);
	element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "value", _size);
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

int MyWorldConfig::getSize() const {
	return _size;
}

int MyWorldConfig::getProbabilityChildInheritsFromFather() const {
	return _probabilityChildInheritsFromFather;
}

int MyWorldConfig::getNumVillages() const {
	return _numVillages;
}


} // namespace Examples
