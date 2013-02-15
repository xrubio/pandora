#include "MyAgent.hxx"
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>
#include <iostream>

namespace Tutorial
{

void MyAgent::initNumberOfAnimals() {
	_numberOfAnimals = Engine::GeneralState::statistics().getUniformDistValue(_config.getNumAnimalsMinIni(), _config.getNumAnimalsMaxIni());
}

void MyAgent::initHasCellphone() {
	int percentAgentsWithCellphone = 65;
	int r = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
	if (r < percentAgentsWithCellphone) _hasCellphone = true;
	else _hasCellphone = false;
}

void MyAgent::initMentalWorldRepresentation() { //if first == -1 then second == 0
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			_mentalWorldRepresentation[i][j].first = -1;
			_mentalWorldRepresentation[i][j].second = Engine::GeneralState::statistics().getUniformDistValue(0, 2);
		}
	}
}

void MyAgent::initCooperationTreat() {
	_cooperationTreat = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
}

void MyAgent::initCellphoneUsage() {
	if (not _hasCellphone) _cellphoneUsage = 0;
	else _cellphoneUsage = Engine::GeneralState::statistics().getUniformDistValue(1, 7);
}

void MyAgent::initSocialNetwork() { //Siempre pone 2 en la relacion, tiene que ir en las dos direcciones!
	std::vector<int> idsAgentsSameVillage = _village.getCitizens();
	for (int i = 0; i < idsAgentsSameVillage.size(); ++i) {
		//if no es el mismo y no esta añadido
		std::pair<int, int> association;
		association.first = idsAgentsSameVillage[i];
		association.second = 2;
		_socialNetwork.push_back(association);
	}
}

void MyAgent::initPosition() {
	setPosition(_village.getLocation());
}

void MyAgent::initWorld(MyWorld *w) {
	_world = w;
}

void MyAgent::initReputation() {
	_reputation = -1;
}

void MyAgent::initCallsMade() {
	_callsMade = 0;
}

void MyAgent::initCellsSharedPerCall() {
	_avgCellsSharedPerCall = -1;
}

void MyAgent::setVillage(MyVillage &v) {
	_village = v;
}

MyAgent::MyAgent(const std::string &id, const Examples::MyWorldConfig &config, MyWorld* w) : Agent(id), _gatheredResources(0) {
	_config = config;	
	initWorld(w);
	initNumberOfAnimals();
	initHasCellphone();
	initMentalWorldRepresentation();
	initCooperationTreat();
	initCellphoneUsage();
	initSocialNetwork();
	initReputation();
	initCallsMade();
	initCellsSharedPerCall();
}

MyAgent::~MyAgent() {
}


void MyAgent::resetNumberOfResourcesGathered() {
	_gatheredResources = 0;
}

void MyAgent::updateNumberOfAnimals() { //TODO depende de definicion del clima

}

void MyAgent::gatherResources() { //TODO depende de definicion del clima
	_gatheredResources += _world->getValueStr("resources", _position);
	_world->setValueStr("resources", _position, 0);
}

bool MyAgent::hasMinimumNumberOfAnimals() {
	return _numberOfAnimals >= 100;
}

void MyAgent::updateMentalWorldRepresentation() {
	for (int i = 0; i < 60; ++i) {
		for (int j = 0; j < 60; ++j) {
			if (_mentalWorldRepresentation[i][j].first != -1) ++_mentalWorldRepresentation[i][j].second;
			if (_mentalWorldRepresentation[i][j].second >= 3) {
				_mentalWorldRepresentation[i][j].first = -1;
				_mentalWorldRepresentation[i][j].second = 0;
			}
		}
	}
}

void MyAgent::updateMentalWorldRepresentationAccuracy() {
	double accuracy = 0;
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			Engine::Point2D<int> position;
			position._x = i;
			position._y = j;
			int actualResources = _world->getValueStr("resources", position);
			int resourcesLevel;
			if (actualResources <= 3) resourcesLevel = 0;
			else if (actualResources > 3 and actualResources <= 6) resourcesLevel = 1;
			else resourcesLevel = 2;
			if (resourcesLevel == _mentalWorldRepresentation[i][j].first) ++accuracy;
		}
	}
	accuracy /= (_config.getSize()*_config.getSize());
	_mentalWorldRepresentationAccuracy = accuracy;
}

void MyAgent::updateReputation() {
	_reputation = (_avgCellsSharedPerCall*100)/_world->getMaximumAvgCellsSharedPerCall();
}

void MyAgent::updateAvgSharedCellsPerCall(int sharedCells) {
	if (_avgCellsSharedPerCall == -1) _avgCellsSharedPerCall = sharedCells;
	else _avgCellsSharedPerCall = (_avgCellsSharedPerCall*_callsMade + sharedCells)/(_callsMade + 1);
}

void MyAgent::fission() {
	//initialize child agent
	std::string idNewAgent = _world->createAgent(_village.getId());
	MyAgent* newAgent = (MyAgent*)_world->getAgent(idNewAgent);
	newAgent->setGatheredResources(0);
	newAgent->setNumberOfAnimals(_numberOfAnimals/2);

	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	//if the child inherits the characteristics of the father
	if (r <= _config.getProbabilityChildInheritsFromFather()) {
		newAgent->setHasCellphone(_hasCellphone);
		newAgent->updateMentalWorldRepresentationAccuracy();
		newAgent->setCooperationTreat(_cooperationTreat);
		newAgent->setCellphoneUsage(_cellphoneUsage);
		//TODO copiar mentalworldrepresentation desde el padre
	}

	_numberOfAnimals -= _numberOfAnimals/2; //the parent has half of the animals than before the fission
}

void MyAgent::fussion() { //TODO
	//_exists = 0;
}

bool MyAgent::agentFissions() {
	double probability = ((_numberOfAnimals-_config.getNumAnimalsMin()*2)*100)/(_config.getNumAnimalsMax()-_config.getNumAnimalsMin()*2);
	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	return r <= probability;
}

bool MyAgent::agentFussions() { //TODO
	
}

void MyAgent::move() {
	Engine::Point2D<int> newPosition = _position;
	newPosition._x += Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	newPosition._y += Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	if(_world->checkPosition(newPosition)) setPosition(newPosition);
}

void MyAgent::setGatheredResources(int resources) {
	_gatheredResources = resources;
}

void MyAgent::setNumberOfAnimals(int animals) {
	_numberOfAnimals = animals;
}

void MyAgent::setHasCellphone(bool cellphone) {
	_hasCellphone = cellphone;
}

void MyAgent::setCooperationTreat(int cooperationTreat) {
	_cooperationTreat = cooperationTreat;
}

void MyAgent::setCellphoneUsage(int cellphoneUsage) {
	_cellphoneUsage = cellphoneUsage;
}

double MyAgent::getAvgCellsSharedPerCall() {
	return _avgCellsSharedPerCall;
}

void MyAgent::updateState() {
	//checkConditions(); where to call this function from?
	move();
	gatherResources();
}

void MyAgent::serialize() { //TODO falta completar
	serializeAttribute("resources", _gatheredResources);
	serializeAttribute("numberOfAnimals", _numberOfAnimals);
	serializeAttribute("hasCellphone", _hasCellphone);
	serializeAttribute("cooperationTreat", _cooperationTreat);
	serializeAttribute("cellphoneUsage", _cellphoneUsage);
}

void MyAgent::registerAttributes() { //TODO falta completar
	registerIntAttribute("resources");
	registerIntAttribute("numberOfAnimals");
	registerIntAttribute("hasCellphone");
	registerIntAttribute("cooperationTreat");
	registerIntAttribute("cellphoneUsage");
}

void MyAgent::checkConditions() { //TODO falta completar
	assert (_numberOfAnimals >= _config.getNumAnimalsMin() and _numberOfAnimals <= _config.getNumAnimalsMax());
}

} // namespace Tutorial
