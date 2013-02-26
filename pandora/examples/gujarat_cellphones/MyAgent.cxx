#include "MyAgent.hxx"
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>
#include <iostream>
#include <map>
#include <stack>

namespace Tutorial
{

MyAgent::MyAgent(const std::string &id, const Examples::MyWorldConfig &config, MyWorld* w, bool initialAgent) : Agent(id), _gatheredResources(0) {
	_config = config;	
	initWorld(w);
	initNumberOfAnimals();
	initHasCellphone();
	initMentalWorldRepresentation();
	initCooperationTreat();
	initCellphoneUsage();
	if (not initialAgent) initSocialNetwork();
	initReputation();
	initMadeCalls();
	initCellsSharedPerCall();
	initLastCalls();
}

MyAgent::~MyAgent() {
}

std::vector<Engine::Point2D<int> > shuffleVector(const std::vector<Engine::Point2D<int> > &v);

void MyAgent::addSpokeInVillage(std::string id) {
	_spokeInVillage.push_back(id);
}

bool MyAgent::agentFissions() {
	double probability = ((_numberOfAnimals-_config.getNumAnimalsMin()*2)*100)/(_config.getNumAnimalsMax()-_config.getNumAnimalsMin()*2);
	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	return r <= probability;
}

bool MyAgent::agentIsInHisVillage() {
	Engine::Point2D<int> villageLocation = _village.getLocation();
	return _position._x == villageLocation._x and _position._y == villageLocation._y;
}

void MyAgent::callMade(std::string id) {
	_lastCalls[id] = 0;
}

void MyAgent::checkConditions() { //TODO falta completar
	assert (_numberOfAnimals >= _config.getNumAnimalsMin() and _numberOfAnimals <= _config.getNumAnimalsMax());
	assert (_cooperationTreat >= 0 and _cooperationTreat <= 100);
	assert (_avgCellsSharedPerCall <= (_config.getMaxPercentMapSharedInACall()*_config.getSize()));
	assert (_reputation <= 100);
	if (not _hasCellphone) assert (_cellphoneUsage == 0);
	else assert (_cellphoneUsage >= _config.getCellphoneUsageMin() and _cellphoneUsage <= _config.getCellphoneUsageMax());
}

std::string MyAgent::chooseWhoToCall() {
	int affinityCallReceiver;
	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	if (r <= _config.getProbabilityCallLowAffinity()) affinityCallReceiver = 0;
	else if (r <= _config.getProbabilityCallMediumAffinity()) affinityCallReceiver = 1;
	else affinityCallReceiver = 2;
	std::vector<std::string> candidates;
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		if (_socialNetwork[i].second == affinityCallReceiver) {
			MyAgent* receiver = (MyAgent*) _world->getAgent(_socialNetwork[i].first);
			if (receiver->hasCellphone()) candidates.push_back(_socialNetwork[i].first);
		}
	}
	if (candidates.size() == 0) return "";
	r = Engine::GeneralState::statistics().getUniformDistValue(0,candidates.size() - 1);
	return candidates[r];
	return "";
}

void MyAgent::createAffinity(std::string id, int affinityLevel) {
	_socialNetwork.push_back(std::pair<std::string, int> (id, affinityLevel));
	_lastCalls[id] = 0;
}

bool MyAgent::decideToMakeACall() {
	if (not _hasCellphone) return false;
	int probability = (_cellphoneUsage*100)/_config.getCellphoneUsageMax();
	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	return r <= probability;
}

void MyAgent::deleteAffinity(std::string id) { 
	bool found = false;
	for (int i = 0; not found and i < _socialNetwork.size(); ++i) {
		if (_socialNetwork[i].first == id) {
			_socialNetwork.erase(_socialNetwork.begin() + i);
			found = true;
		}
	}
	_lastCalls.erase(id);
}

void MyAgent::exchangeInformationWithOtherAgent(std::string idAgentReceivesCall) {
	MyAgent *agentReceivingCall = (MyAgent*) _world->getAgent(idAgentReceivesCall);
	int ncells = agentReceivingCall->numberOfCellsWillingToTell(_id);
	std::vector<Engine::Point2D<int> > cellsToAsk;
	if (agentIsInHisVillage()) {
		std::vector<std::vector<int> > knownNeighborCells = getMatrixKnownNeighborCells();
		cellsToAsk = getCellsToAskInVillage(knownNeighborCells, ncells*_config.getMultiplierCellsToAsk());
	}
	else cellsToAsk = getCellsToAskOutsideVillage(ncells*_config.getMultiplierCellsToAsk());
	int infoCellsExchanged = 0;
	for (int i = 0; i < cellsToAsk.size() and infoCellsExchanged <= ncells; ++i) {
		if (agentReceivingCall->knowsCell(cellsToAsk[i]._x, cellsToAsk[i]._y)) {
			int cellValue = agentReceivingCall->getValueCellMentalWorldRepresentation(cellsToAsk[i]._x, cellsToAsk[i]._y);
			int cellTime = agentReceivingCall->getTimeCellMentalWorldRepresentation(cellsToAsk[i]._x, cellsToAsk[i]._y);
			updateCellMentalWorldRepresentation(cellsToAsk[i]._x, cellsToAsk[i]._y, cellValue, cellTime);
			++infoCellsExchanged;
		}
	}
	callMade(idAgentReceivesCall);
	agentReceivingCall->updateAvgSharedCellsPerCall(infoCellsExchanged);
}

void MyAgent::exchangeInfoWithPeopleInVillage() {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		MyAgent* a = (MyAgent*) _world->getAgent(_socialNetwork[i].first);
		if (_village.getId() == a->getVillage().getId()) {
			bool haveAlreadySpoken = false;
			for (int j = 0; j < _spokeInVillage.size() and not haveAlreadySpoken; ++j) {
				if (_spokeInVillage[j] == _socialNetwork[i].first) haveAlreadySpoken = true;
			}
			if (not haveAlreadySpoken) {
				bool exchangeInformation = false;
				int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
				if (_socialNetwork[i].second == 2) {
					exchangeInformation = (r <= _config.getProbabilityExchangeInfoHighAffinityInVillage());
				}
				else {
					exchangeInformation = (r <= _config.getProbabilityExchangeInfoMediumAffinityInVillage());
				}
				if (exchangeInformation) {
					exchangeInformationWithOtherAgent(a->getId());
					a->exchangeInformationWithOtherAgent(_id);
					_spokeInVillage.push_back(a->getId());
					a->addSpokeInVillage(_id);
				}
			}
		}
	}
}

void MyAgent::fission() {
	//initialize child agent
	std::string idNewAgent = _world->createAgent(_village.getId(), false);
	MyAgent* newAgent = (MyAgent*)_world->getAgent(idNewAgent);
	newAgent->setGatheredResources(0);
	newAgent->setNumberOfAnimals(_numberOfAnimals/2);

	int r = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	//if the child inherits the characteristics of the father
	if (r <= _config.getProbabilityChildInheritsFromFather()) {
		newAgent->setHasCellphone(_hasCellphone);
		newAgent->setCooperationTreat(_cooperationTreat);
		newAgent->setCellphoneUsage(_cellphoneUsage);
		for (int i = 0; i < _socialNetwork.size(); ++i) {
			MyAgent* a = (MyAgent*)_world->getAgent(_socialNetwork[i].first);
			if (newAgent->hasAffinityWithAgent(_socialNetwork[i].first)) {
				newAgent->deleteAffinity(_socialNetwork[i].first); //the affinity level needs to be the same
				a->deleteAffinity(newAgent->getId());
			}
			newAgent->createAffinity(_socialNetwork[i].first, _socialNetwork[i].second);
			a->createAffinity(newAgent->getId(), _socialNetwork[i].second);
		}
		newAgent->deleteAffinity(_id); //in case it was created randomly
		newAgent->createAffinity(_id, 2);
		for (int i = 0; i < _config.getSize(); ++i) {
			for (int j = 0; j < _config.getSize(); ++j) {
				newAgent->updateCellMentalWorldRepresentation(i, j, _mentalWorldRepresentation[i][j].first, _mentalWorldRepresentation[i][j].second);
			}
		}
	}

	deleteAffinity(idNewAgent); //in case it was created randomly
	createAffinity(idNewAgent, 2);
	_numberOfAnimals -= _numberOfAnimals/2; //the parent has half of the animals than before the fission
}

void MyAgent::gatherResources() { //TODO depende de definicion del clima
	_gatheredResources += _world->getValueStr("resources", _position);
	_world->setValueStr("resources", _position, 0);
}

int MyAgent::getAffinityWithAgent(std::string id) {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		if (_socialNetwork[i].first == id) return _socialNetwork[i].second;
	}
}

double MyAgent::getAvgCellsSharedPerCall() {
	return _avgCellsSharedPerCall;
}

int MyAgent::getCellphoneUsage() {
	return _cellphoneUsage;
}

std::vector<Engine::Point2D<int> > MyAgent::getCellsToAskInVillage(const std::vector<std::vector<int> > &m, int numberOfCells) {
	std::vector<Engine::Point2D<int> > cellsToAsk;
	int max = 0;
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			if (m[i][j] > max) max = m[i][j];
		}
	}
	while (max > 0 and (cellsToAsk.size() < numberOfCells)) {
		std::vector<Engine::Point2D<int> > aux;
		for (int i = 0; i < _config.getSize(); ++i) {
			for (int j = 0; j < _config.getSize(); ++j) {
				if (m[i][j] == max) aux.push_back(Engine::Point2D<int> (i, j)); 
			}
		}
		aux = shuffleVector(aux);
		for (int i = 0; i < aux.size() and (cellsToAsk.size() < numberOfCells); ++i) {
			cellsToAsk.push_back(aux[i]);
		}
		--max;
	}
	return cellsToAsk;
}

std::vector<Engine::Point2D<int> > MyAgent::getCellsToAskOutsideVillage(int numberOfCells) {
	std::vector<Engine::Point2D<int> > cellsToAsk;
	for (int i = _position._x - 2; i <= _position._x + 2; ++i) {
		for (int j = _position._y - 2; j <= _position._y + 2; ++j) {
			if ((i != _position._x or j != _position._y) and _world->checkPosition(Engine::Point2D<int> (i, j))) cellsToAsk.push_back(Engine::Point2D<int> (i, j));
		}
	}
	cellsToAsk = shuffleVector(cellsToAsk);
	if (numberOfCells >= cellsToAsk.size()) return cellsToAsk;
	while (numberOfCells < cellsToAsk.size()) {
		cellsToAsk.erase(cellsToAsk.begin() + cellsToAsk.size() - 1);
	}
	return cellsToAsk;
}

int MyAgent::getCooperationTreat() {
	return _cooperationTreat;
}

int MyAgent::getMadeCalls() {
	return _madeCalls;
}

int MyAgent::getNumberOfAnimals() {
	return _numberOfAnimals;
}

int MyAgent::getGatheredResources() {
	return _gatheredResources;
}

bool MyAgent::getHasCellphone() {
	return _hasCellphone;
}

std::string MyAgent::getId() {
	return _id;
}

int MyAgent::getLastCall(std::string id) {
	return _lastCalls[id];
}

std::map<std::string, int>  MyAgent::getLastCalls() {
	return _lastCalls;
}

std::vector<std::vector<int> > MyAgent::getMatrixKnownNeighborCells() {
	std::vector<std::vector<int> > result(_config.getSize(), std::vector<int>(_config.getSize()));
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			if (_mentalWorldRepresentation[i][j].first != -1) result[i][j] = -1;
			else result[i][j] = longestPathKnownCellsFromCell(i, j);
		}
	}	
	return result;
}

int MyAgent::getReputation() {
	return _reputation;
}

std::vector<std::pair<std::string,int> > MyAgent::getSocialNetwork() {
	return _socialNetwork;
}

int MyAgent::getTimeCellMentalWorldRepresentation(int x, int y) {
	return _mentalWorldRepresentation[x][y].second;
}

std::vector<Engine::Point2D<int> > MyAgent::getUnknownNeighborCells(int x, int y) {
	std::vector<Engine::Point2D<int> > neighborCells;
	for (int i = x - 1; i <= x + 1; ++i) {
		for (int j = y - 1; j <= y + 1; ++j) {
			Engine::Point2D<int> newPosition;
			newPosition._x = i;
			newPosition._y = j;
			if (_world->checkPosition(newPosition) and (i != y or j != x) and _mentalWorldRepresentation[i][j].first == -1) {
				neighborCells.push_back(newPosition);
			}
		}
	}
	return shuffleVector(neighborCells);
}

int MyAgent::getValueCellMentalWorldRepresentation(int x, int y) {
	return _mentalWorldRepresentation[x][y].first;
}

MyVillage MyAgent::getVillage() {
	return _village;
}

bool MyAgent::hasAffinityWithAgent(std::string id) {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		if (_socialNetwork[i].first == id) return true;
	}
	return false;
}

bool MyAgent::hasCellphone() {
	return _hasCellphone;
}

bool MyAgent::hasMinimumNumOfAnimals() {
	return _numberOfAnimals >= _config.getNumAnimalsMin();
}

void MyAgent::initCellphoneUsage() {
	if (not _hasCellphone) _cellphoneUsage = 0;
	else _cellphoneUsage = Engine::GeneralState::statistics().getUniformDistValue(_config.getCellphoneUsageMin(), _config.getCellphoneUsageMax());
}

void MyAgent::initCellsSharedPerCall() {
	_avgCellsSharedPerCall = -1;
}

void MyAgent::initCooperationTreat() {
	_cooperationTreat = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
}

void MyAgent::initHasCellphone() {
	int r = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
	_hasCellphone = (r < _config.getPercentAgentsWithCellphone());
}

void MyAgent::initLastCalls() {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		_lastCalls[_socialNetwork[i].first] = 0;
	}
}

void MyAgent::initMadeCalls() {
	_madeCalls = 0;
}

void MyAgent::initMentalWorldRepresentation() {
	//All cells are unknown
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			_mentalWorldRepresentation[i][j].first = -1;
			_mentalWorldRepresentation[i][j].second = -1;
		}
	}
	//Initialize some cells
	for (int i = 0; i < _config.getMaxYearsCellInfo(); ++i) {
		int cellsNeeded = (_config.getPercentMapKnownAtBeginning()*_config.getSize()*_config.getSize())/(_config.getMaxYearsCellInfo()*100);
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

void MyAgent::initNumberOfAnimals() {
	_numberOfAnimals = Engine::GeneralState::statistics().getUniformDistValue(_config.getNumAnimalsMinIni(), _config.getNumAnimalsMaxIni());
}

void MyAgent::initPosition() {
	setPosition(_village.getLocation());
}

void MyAgent::initReputation() {
	_reputation = -1;
}

void MyAgent::initSocialNetwork() { 
	std::vector<std::string> idsExistingAgents = _world->getIdsExistingAgents();
	for (int i = 0; i < idsExistingAgents.size(); ++i) {
		MyAgent* a = (MyAgent*)_world->getAgent(idsExistingAgents[i]);
		if (idsExistingAgents[i] != _id) {
			if (_village.getId() == a->getVillage().getId()) {
				int r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
				if (r <= _config.getProbabilityKnowAgentSameVillageAtStart()) {
					r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
					if (r <= _config.getPercentKnownAgentsSameVillageHighAffinity()) {
						createAffinity(idsExistingAgents[i], 2);
						a->createAffinity(_id, 2);
					}
					else {
						createAffinity(idsExistingAgents[i], 1);
						a->createAffinity(_id, 1);
					}
				}
			}
			else {
				int r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
				if (r <= _config.getProbabilityKnowAgentDifferentVillageAtStart()) {
					r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
					if (r <= _config.getPercentKnownAgentsDifferentVillageMediumAffinity()) {
						createAffinity(idsExistingAgents[i], 1);
						a->createAffinity(_id, 1);
					}
					else {
						createAffinity(idsExistingAgents[i], 0);
						a->createAffinity(_id, 0);
					}
				}
			}
		}
	}
}

void MyAgent::initWorld(MyWorld *w) {
	_world = w;
}

bool MyAgent::knowsCell(int i, int j) {
	return _mentalWorldRepresentation[i][j].first != -1;
}

int MyAgent::longestPathKnownCellsFromCell(int i, int j) {
	int longestPath = 0;
	std::vector<std::vector<bool> > visited(_config.getSize(), std::vector<bool>(_config.getSize(), false));
	visited[i][j] = true;
	std::stack<Engine::Point2D<int> > s;
	s.push(Engine::Point2D<int> (i, j));
	while (not s.empty()) {
		Engine::Point2D<int> p = s.top();
		s.pop();
		if (_mentalWorldRepresentation[p._x][p._y].first != -1) ++longestPath;	
		for (int a = p._x - 1; a <= p._x + 1; ++a) {
			for (int b = p._y - 1; b <= p._y + 1; ++b) {
				if ((a >= 0 and a < _config.getSize()) and (b >= 0 and b < _config.getSize()) and (not (p._x == a and p._y == b)) and (_mentalWorldRepresentation[a][b].first != -1) and (not visited[a][b])){			
					s.push(Engine::Point2D<int> (a, b));
					visited[a][b] = true;
				}
			}
		}
	}
	return longestPath;
}

void MyAgent::meetAgentsInSameCell() {
	std::vector<std::string> agentsIds = _world->getIdsAgentsInCell(_position._x, _position._y);
	for (int i = 0; i < agentsIds.size(); ++i) {
		MyAgent* a = (MyAgent*) _world->getAgent(agentsIds[i]);
		if (not hasAffinityWithAgent(agentsIds[i])) {
			int r = Engine::GeneralState::statistics().getUniformDistValue(0, 100);
			if (r <= _config.getProbabilityMeetAgentSameCell()) {
				if (_village.getId() == a->getVillage().getId()) {
					createAffinity(a->getId(), 1);
					a->deleteAffinity(_id); 
					a->createAffinity(_id, 1);
				}
				else {
					createAffinity(a->getId(), 0);
					a->deleteAffinity(_id); 
					a->createAffinity(_id, 0);
				}
			}
		}
		if (hasAffinityWithAgent(agentsIds[i])) {
			exchangeInformationWithOtherAgent(agentsIds[i]);
			a->exchangeInformationWithOtherAgent(_id);
		}
	}
}

void MyAgent::move() {
	int newx = _position._x + Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	int newy = _position._y + Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	if ((newx >= 0) and (newx < _config.getSize()) and (newy >= 0) and (newy < _config.getSize())) {
		if (newx != _position._x and newy != _position._y) updateMentalWorldRepresentation();
		setPosition(Engine::Point2D<int> (newx, newy));
	}
}

int MyAgent::numberOfCellsWillingToTell(std::string idReceivingAgent) {
	MyAgent* receivingAgent = (MyAgent*) _world->getAgent(idReceivingAgent);
	int affinity = getAffinityWithAgent(idReceivingAgent);
	int max = _config.getSize()*_config.getSize()*(_config.getMaxPercentMapSharedInACall()/(1000.0));
	if (affinity == 2) return max*(0.8 + 0.2*(receivingAgent->getCooperationTreat()/100.0));
	else if (affinity == 1) {
		if (_reputation <= receivingAgent->getReputation()) {
			return max*(0.5 + 0.2*(receivingAgent->getCooperationTreat()/100.0));
		}
		return max*0.5;
	}
	else {
		if (_reputation <= receivingAgent->getReputation()) {
			return max*(0.1 + 0.2*(receivingAgent->getCooperationTreat()/100.0));
		}
		return max*0.1;
	}
}

void MyAgent::registerAttributes() { //TODO falta completar
	registerIntAttribute("cellphoneUsage");
	registerIntAttribute("cooperationTreat");
	registerIntAttribute("hasCellphone");
	registerIntAttribute("numberOfAnimals");
	registerIntAttribute("reputation");
	registerIntAttribute("resources");
}

void MyAgent::resetNumberOfResourcesGathered() {
	_gatheredResources = 0;
}

void MyAgent::resetSpokeInVillage() {
	_spokeInVillage.clear();
}

void MyAgent::returnToVillage() { //TODO funcion provisional hasta que markov agent este completado
	Engine::Point2D<int> villagePosition = _village.getLocation();
	_position._x = villagePosition._x;
	_position._y = villagePosition._y;
}

void MyAgent::serialize() { //TODO falta completar
	serializeAttribute("cellphoneUsage", _cellphoneUsage);
	serializeAttribute("cooperationTreat", _cooperationTreat);
	serializeAttribute("hasCellphone", _hasCellphone);
	serializeAttribute("numberOfAnimals", _numberOfAnimals);
	serializeAttribute("reputation", _reputation);
	serializeAttribute("resources", _gatheredResources);
}

void MyAgent::setCellphoneUsage(int cellphoneUsage) {
	_cellphoneUsage = cellphoneUsage;
}

void MyAgent::setCooperationTreat(int cooperationTreat) {
	_cooperationTreat = cooperationTreat;
}

void MyAgent::setGatheredResources(int resources) {
	_gatheredResources = resources;
}

void MyAgent::setHasCellphone(bool cellphone) {
	_hasCellphone = cellphone;
}

void MyAgent::setLastCall(std::string id, int days) {
	bool inSocialNetwork = false;
	for (int i = 0; i < _socialNetwork.size() and not inSocialNetwork; ++i) {
		if (_socialNetwork[i].first == id) inSocialNetwork = true;
	}
	if (inSocialNetwork) {
		_lastCalls[id] = days;
	}
}

void MyAgent::setNumberOfAnimals(int animals) {
	_numberOfAnimals = animals;
}

void MyAgent::setVillage(MyVillage &v) {
	_village = v;
}

std::vector<Engine::Point2D<int> > shuffleVector(const std::vector<Engine::Point2D<int> > &v) {
	std::vector<Engine::Point2D<int> > shuffled(v.size());
	std::vector<Engine::Point2D<int> > aux(v.size());
	for (int i = 0; i < v.size(); ++i) aux[i] = v[i];
	for (int i = 0; i < v.size(); ++i) {
		int r =Engine::GeneralState::statistics().getUniformDistValue(0, aux.size()-1);
		shuffled[i] = aux[r];
		aux.erase(aux.begin() + r);
	}
	return shuffled;
}

void MyAgent::stopBeingAShepherd() {
	//delete the agent in the social network of the rest of agents
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		MyAgent* contact = (MyAgent*)_world->getAgent(_socialNetwork[i].first);
		contact->deleteAffinity(_id);
	}
	_exists = false;
	_world->addDeletedAgent(_id);
}

void MyAgent::updateAffinities() {
	std::vector<std::string> affinitiesToDelete;
	MyAgent* a;
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		a = (MyAgent*)_world->getAgent(_socialNetwork[i].first);
		int daysLastCall = getLastCall(_socialNetwork[i].first);
		if (_village.getId() == a->getVillage().getId()) {
			if (daysLastCall <= _config.getSameVillageDaysNoCallsHigh()) _socialNetwork[i].second = 2;
			else if (daysLastCall <= _config.getSameVillageDaysNoCallsMedium()) _socialNetwork[i].second = 1;
			else affinitiesToDelete.push_back(_socialNetwork[i].first);
		}
		else {
			if (daysLastCall <= _config.getDiffVillageDaysNoCallsMedium()) _socialNetwork[i].second = 1;
			else if (daysLastCall <= _config.getDiffVillageDaysNoCallsLow()) _socialNetwork[i].second = 0;
			else affinitiesToDelete.push_back(_socialNetwork[i].first);
		}
	}
	for (int i = 0; i < affinitiesToDelete.size(); ++i) {
		a = (MyAgent*)_world->getAgent(affinitiesToDelete[i]);
		deleteAffinity(affinitiesToDelete[i]);
		a->deleteAffinity(_id);
	}
}

void MyAgent::updateAvgSharedCellsPerCall(int sharedCells) {
	if (_avgCellsSharedPerCall == -1) _avgCellsSharedPerCall = sharedCells;
	else _avgCellsSharedPerCall = (_avgCellsSharedPerCall*_madeCalls + sharedCells)/(_madeCalls + 1);
}

void MyAgent::updateCellMentalWorldRepresentation(int x, int y, int resourcesLevel, int time) {
	//update only if the previous information is older or if it does not exist
	if (_mentalWorldRepresentation[x][y].first == -1 or _mentalWorldRepresentation[x][y].second > time) {
		_mentalWorldRepresentation[x][y].first = resourcesLevel;
		_mentalWorldRepresentation[x][y].second = time;
	}
}

void MyAgent::updateLastCalls() {
	for (int i = 0; i < _socialNetwork.size(); ++i) {
		++_lastCalls[_socialNetwork[i].first];
	}
}

void MyAgent::updateMentalWorldRepresentation() {
	Engine::Point2D<int> position = getPosition();
	int actualResources = _world->getValueStr("resources", position);
	if (actualResources <= _config.getResourcesLowLevel()) updateCellMentalWorldRepresentation(position._x, position._y, 0, 0);
	else if (actualResources >= _config.getResourcesHighLevel()) updateCellMentalWorldRepresentation(position._x, position._y, 2, 0);
	else updateCellMentalWorldRepresentation(position._x, position._y, 1, 0);
}

void MyAgent::updateNumberOfAnimals() { //TODO depende de definicion del clima

}

void MyAgent::updateReputation() {
	if (_world->getMaximumAvgCellsSharedPerCall() == -1) _reputation = 100;
	else _reputation = (_avgCellsSharedPerCall*100)/_world->getMaximumAvgCellsSharedPerCall();
}

void MyAgent::updateState() {
	if (_exists) {
		int dayOfYear = (_world->getCurrentStep())%(_config.getDaysDrySeason() + _config.getDaysWetSeason());
		if (dayOfYear == 0 and _world->getCurrentStep() != 0) { //first day of wet season
			returnToVillage(); //TODO provisional hasta que markov agent este hecho
			updateYearsMentalWorldRepresentation();
			updateNumberOfAnimals();
			if (agentFissions()) fission();
			resetNumberOfResourcesGathered();
		}
		else if (dayOfYear == _config.getDaysWetSeason()) { //last day wet season
			exchangeInfoWithPeopleInVillage();
			resetSpokeInVillage();
		}
		else if (dayOfYear >= _config.getDaysWetSeason()){ //dry season
			move();
			gatherResources();
			meetAgentsInSameCell();
		}
		if (decideToMakeACall()) {
			std::string whoToCall = chooseWhoToCall();
			if (whoToCall != "") {
				exchangeInformationWithOtherAgent(whoToCall);
				MyAgent* receiver = (MyAgent*)_world->getAgent(whoToCall);
				receiver->exchangeInformationWithOtherAgent(_id);
			}
		}
		updateLastCalls();
		updateReputation();
		updateAffinities();
		if (dayOfYear == 0) {
			if (not hasMinimumNumOfAnimals()) stopBeingAShepherd();
		}
		if (_exists) checkConditions();
	}
}

void MyAgent::updateYearsMentalWorldRepresentation() {
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

} // namespace Tutorial
