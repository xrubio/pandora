#include "MyAgent.hxx"
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>
#include <iostream>
#include <stack>

namespace Tutorial
{

void MyAgent::initNumberOfAnimals() {
	_numberOfAnimals = Engine::GeneralState::statistics().getUniformDistValue(_config.getNumAnimalsMinIni(), _config.getNumAnimalsMaxIni());
}

void MyAgent::initHasCellphone() {
	int percentAgentsWithCellphone = 65;
	int r = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
	_hasCellphone = (r < percentAgentsWithCellphone);
}

void MyAgent::updateCellMentalWorldRepresentation(int x, int y, int resourcesLevel, int time) {
	_mentalWorldRepresentation[x][y].first = resourcesLevel;
	_mentalWorldRepresentation[x][y].second = time;
}

std::vector<Engine::Point2D<int> > MyAgent::getUnknownNeighborCells(int x, int y) {
	std::vector<Engine::Point2D<int> > neighborCells;
	for (int i = y-1; i <= y+1; ++i) {
		for (int j = x-1; j <= x+1; ++j) {
			Engine::Point2D<int> newPosition;
			newPosition._x = j;
			newPosition._y = i;
			if (_world->checkPosition(newPosition) and not (i == y and j == x) and _mentalWorldRepresentation[i][j].first != -1) {
				neighborCells.push_back(newPosition);
			}
		}
	}
	return neighborCells;
}

void MyAgent::initMentalWorldRepresentation() { //TODO completar
	//All cells are unknown
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			_mentalWorldRepresentation[i][j].first = -1;
			_mentalWorldRepresentation[i][j].second = -1;
		}
	}
	//Initialize some cells
	for (int i = 0; i < _config.getMaxYearsCellInfo(); ++i) {
		int cellsNeeded = _config.getPercentMapKnownAtBeginning()/_config.getMaxYearsCellInfo();
		int cellsFilled = 0;
		Engine::Point2D<int> position;
		position._x = Engine::GeneralState::statistics().getUniformDistValue(0, _config.getSize()-1);
		position._y = Engine::GeneralState::statistics().getUniformDistValue(0, _config.getSize()-1);
		std::stack<Engine::Point2D<int> > positionsToExplore;
		positionsToExplore.push(position);
		while (cellsFilled < cellsNeeded and not positionsToExplore.empty()) {
			position = positionsToExplore.top();
			positionsToExplore.pop();
			int actualResources = _world->getValueStr("resources", position);
			int resourcesLevel;
			if (actualResources <= _config.getResourcesLowLevel()) resourcesLevel = 0;
			else if (actualResources >= _config.getResourcesHighLevel()) resourcesLevel = 2;
			else resourcesLevel = 1;
			updateCellMentalWorldRepresentation(position._x, position._y, resourcesLevel, i);
			std::vector<Engine::Point2D<int> > neighborCells = getUnknownNeighborCells(position._x, position._y);
			for (int j = 0; j < neighborCells.size(); ++j) positionsToExplore.push(neighborCells[j]);
			++cellsFilled;
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
	//usar nueva funcion createAffinity
	std::vector<std::string> idsAgentsSameVillage = _village.getCitizens();
	for (int i = 0; i < idsAgentsSameVillage.size(); ++i) {
		//if no es el mismo y no esta añadido
		std::pair<std::string, int> association;
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

void MyAgent::initMadeCalls() {
	_madeCalls = 0;
}

void MyAgent::initCellsSharedPerCall() {
	_avgCellsSharedPerCall = -1;
}

void MyAgent::initLastCalls() {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		_lastCalls.push_back(std::pair<std::string, int> (_socialNetwork[i].first, 0));
	}
}

void MyAgent::setVillage(MyVillage &v) {
	_village = v;
}

MyVillage MyAgent::getVillage() {
	return _village;
}

void MyAgent::callMade(std::string id) {
	for (int i = 0; i < _lastCalls.size(); ++i) {
		if (_lastCalls[i].first == id) _lastCalls[i].second = 0;
	}
	MyAgent* receiver = (MyAgent*)_world->getAgent(id);
	receiver->callMade(_id);
}

void MyAgent::updateLastCalls() {
	for (int i = 0; i < _lastCalls.size(); ++i) {
		++_lastCalls[i].second;
	}
}

void MyAgent::createAffinity(std::string id, int affinityLevel) {
	_socialNetwork.push_back(std::pair<std::string, int> (id, affinityLevel));
	_lastCalls.push_back(std::pair<std::string, int> (id, 0));
}

void MyAgent::deleteAffinity(std::string id) { //TODO
	bool found = false;
	for (int i = 0; not found and i < _socialNetwork.size(); ++i) {
		if (_socialNetwork[i].first == id) {
			_socialNetwork.erase(_socialNetwork.begin(), _socialNetwork.begin() + i);
			found = true;
		}
	}
}

void MyAgent::updateAffinities() { //TODO
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		MyAgent* a = (MyAgent*)_world->getAgent(_socialNetwork[i].first);
		if (_village.getId() == a->getVillage().getId()) {
			/*if () _socialNetwork[i].second = 2;
			else if () _socialNetwork[i].second = 1;
			else {
				deleteAffinity(_socialNetwork[i].first);//delete x2
				a->deleteAffinity(_id);
			}*/
		}
		else {
			/*if () _socialNetwork[i].second = 1;
			else if () _socialNetwork[i].second = 0;
			else {
				deleteAffinity(_socialNetwork[i].first);//delete x2
				a->deleteAffinity(_id);
			}*/
		}
	}
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
	initMadeCalls();
	initCellsSharedPerCall();
	initLastCalls();
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

void MyAgent::updateMentalWorldRepresentation() {
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			if (_mentalWorldRepresentation[i][j].first != -1) ++_mentalWorldRepresentation[i][j].second;
			if (_mentalWorldRepresentation[i][j].second >= _config.getMaxYearsCellInfo()) {
				_mentalWorldRepresentation[i][j].first = -1;
				_mentalWorldRepresentation[i][j].second = -1;
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
	else _avgCellsSharedPerCall = (_avgCellsSharedPerCall*_madeCalls + sharedCells)/(_madeCalls + 1);
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
		//TODO copiar mentalworldrepresentation desde el padre, añadir al padre a la red social del hijo
	}

	_numberOfAnimals -= _numberOfAnimals/2; //the parent has half of the animals than before the fission
}

void MyAgent::deleteContactInSocialNetwork(std::string id) {
	bool found = false;
	for (int i = 0; not found and i < _socialNetwork.size(); ++i) {
		if (_socialNetwork[i].first == id) {
			_socialNetwork.erase(_socialNetwork.begin() + i);
			found = true;
		}
	}
}

void MyAgent::stopBeingAShepherd() {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		MyAgent* contact = (MyAgent*)_world->getAgent(_socialNetwork[i].first);
		contact->deleteContactInSocialNetwork(_id);
	}
	_exists = 0;
}

bool MyAgent::agentFissions() {
	double probability = ((_numberOfAnimals-_config.getNumAnimalsMin()*2)*100)/(_config.getNumAnimalsMax()-_config.getNumAnimalsMin()*2);
	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	return r <= probability;
}

bool MyAgent::hasMinimumNumOfAnimals() {
	return _numberOfAnimals <= _config.getNumAnimalsMin();
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
