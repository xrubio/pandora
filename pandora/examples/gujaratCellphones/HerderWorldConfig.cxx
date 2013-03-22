#include <HerderWorldConfig.hxx>

namespace GujaratCellphones
{

HerderWorldConfig::HerderWorldConfig() {
}

HerderWorldConfig::~HerderWorldConfig() {
}

void HerderWorldConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = 0;
	TiXmlElement * child = 0;

	//environment
	element = root->FirstChildElement("environment");
	child = element->FirstChildElement("landscape");
	retrieveAttributeMandatory(child, "size", _size);
	retrieveAttributeMandatory(child, "averageResources", _averageResources);

	child = element->FirstChildElement("climate");
	retrieveAttributeMandatory(child, "seed", _climateSeed);
	retrieveAttributeMandatory(child, "daysDrySeason", _daysDrySeason);
	retrieveAttributeMandatory(child, "meanRain", _rainHistoricalDistribMean);
	retrieveAttributeMandatory(child, "stddev", _rainHistoricalDistribStdDev);

	//population
	element = root->FirstChildElement("population");
	child = element->FirstChildElement("villages");
	retrieveAttributeMandatory(child, "num", _numVillages);
	retrieveAttributeMandatory(child, "agentsPerVillage", _numAgentsPerVillage);
	child = element->FirstChildElement("animals");	
	retrieveAttributeMandatory(child, "numPerHerder", _animalsPerHerder);
	retrieveAttributeMandatory(child, "resourcesPerDay", _resourcesNeededPerAnimal);	

	//mdp
	element = root->FirstChildElement("mdp");
	retrieveAttributeMandatory(element, "horizon", _horizon);
	retrieveAttributeMandatory(element, "width", _width);	
	retrieveAttributeMandatory(element, "explorationBonus", _explorationBonus);	


	//knowledge transmission
	element = root->FirstChildElement("knowledgeTransmission");
	child = element->FirstChildElement("inVillage");	
	retrieveAttributeMandatory(child, "active", _inVillageTransmission);
	retrieveAttributeMandatory(child, "fixedValue", _inVillageTransmissionValue);
	
	child = element->FirstChildElement("outVillage");	
	retrieveAttributeMandatory(child, "active", _outVillageTransmission);
	retrieveAttributeMandatory(child, "fixedValue", _outVillageTransmissionValue);
}

int HerderWorldConfig::getSize() const
{
	return _size;
}

} // namespace GujaratCellphones
