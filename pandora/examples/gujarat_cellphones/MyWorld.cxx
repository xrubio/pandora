#include <MyWorld.hxx>
#include <Simulation.hxx>
#include "MyAgent.hxx"

namespace Tutorial 
{

MyWorld::MyWorld(Engine::Simulation &simulation, Examples::MyWorldConfig &config) : World(simulation, 1, false, "./data/results.h5") {
	_agentsCounter = 0;
	_config = config;
}

MyWorld::~MyWorld() {
}

void MyWorld::addDeletedAgent(std::string id) {
	_idsDeletedAgents.push_back(id);
}

bool MyWorld::agentHasBeenDeleted(std::string id) {
	for (int i = 0; i < _idsDeletedAgents.size(); ++i) {
		if (_idsDeletedAgents[i] == id) return true;
	}
	return false;
}

std::string MyWorld::createAgent(int idVillage, bool initialAgent) {
	std::ostringstream oss;
	oss << "MyAgent_" << _agentsCounter;
	MyAgent* agent = new MyAgent(oss.str(), _config, this, initialAgent);
	addAgent(agent);
	_villages[idVillage].addCitizen(oss.str());
	agent->setVillage(_villages[idVillage]);
	agent->initPosition();
	++_agentsCounter;
	return oss.str();
}

void MyWorld::createAgents() {
	for(int i = 0; i < _config.getNumAgents(); ++i) {
		if((i%_simulation.getNumTasks())==_simulation.getId()) {
			createAgent(Engine::GeneralState::statistics().getUniformDistValue(0,_config.getNumVillages() - 1), true);
		}
	}
	initSocialNetwork();
}

void MyWorld::createRasters() {
	registerDynamicRaster("resources", true);
	getDynamicRasterStr("resources").setInitValues(0, 10, 0);
	createVillages();
}

void MyWorld::createVillages() {
	initVillage(0, _config.getSize()/8, _config.getSize()/8);
	initVillage(1, 3*_config.getSize()/8, 3*_config.getSize()/8);
	initVillage(2, _config.getSize()/8, 5*_config.getSize()/8);
	initVillage(3, 3*_config.getSize()/8, 7*_config.getSize()/8);	
	initVillage(4, 5*_config.getSize()/8, _config.getSize()/8);
	initVillage(5, 7*_config.getSize()/8, 3*_config.getSize()/8);
	initVillage(6, 5*_config.getSize()/8, 5*_config.getSize()/8);
	initVillage(7, 7*_config.getSize()/8, 7*_config.getSize()/8);
}

int MyWorld::getAgentsCounter() {
	return _agentsCounter;
}

std::vector<std::string> MyWorld::getIdsAgentsInCell(int x, int y) {
	std::vector<std::string> idsAgents;
	for (int i = 0; i < _agentsCounter; ++i) {
		std::ostringstream oss;
		oss << "MyAgent_" << i;
		MyAgent* a = (MyAgent*)getAgent(oss.str());
		if (a->exists() and (a->getPosition()._x == x and a->getPosition()._y == y)) {
			idsAgents.push_back(a->getId());
		}
	}
	return idsAgents;
}

std::vector<std::string> MyWorld::getIdsExistingAgents() {
	std::vector<std::string> ids;
	for (int i = 0; i < _agentsCounter; ++i) {
		std::ostringstream oss;
		oss << "MyAgent_" << i;
		MyAgent* a = (MyAgent*)getAgent(oss.str());
		if (not agentHasBeenDeleted(oss.str())) ids.push_back(oss.str());
	}
	return ids;
}

double MyWorld::getMaximumAvgCellsSharedPerCall() { 
	double max = -1;
	std::vector<std::string> ids = getIdsExistingAgents();
	for (int i = 0; i < ids.size(); ++i) {
		MyAgent* a = (MyAgent*)getAgent(ids[i]);
		if (max == -1 or a->getAvgCellsSharedPerCall() > max) {
			max = a->getAvgCellsSharedPerCall();
		}
	}		
	return max;
}

MyVillage MyWorld::getVillage(int id) {
	for (int i = 0; i < _villages.size(); ++i) {
		if (_villages[i].getId() == id) return _villages[i];
	}
}

void MyWorld::initSocialNetwork() {
	std::vector<std::string> agentsIds = getIdsExistingAgents(); 
	for (int i = 0; i < agentsIds.size(); ++i) {
		for (int j = i + 1; j < agentsIds.size(); ++j) {
			MyAgent* a1 = (MyAgent*) getAgent(agentsIds[i]);
			MyAgent* a2 = (MyAgent*) getAgent(agentsIds[j]);
			if (a1->getVillage().getId() == a2->getVillage().getId()) {
				int r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
				if (r <= _config.getProbabilityKnowAgentSameVillageAtStart()) {
					r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
					if (r <= _config.getPercentKnownAgentsSameVillageHighAffinity()) {
						a1->createAffinity(agentsIds[j], 2);
						a2->createAffinity(agentsIds[i], 2);
					}
					else {
						a1->createAffinity(agentsIds[j], 1);
						a2->createAffinity(agentsIds[i], 1);
					}
				}
			}
			else {
				int r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
				if (r <= _config.getProbabilityKnowAgentDifferentVillageAtStart()) {
					r = Engine::GeneralState::statistics().getUniformDistValue(1, 100);
					if (r <= _config.getPercentKnownAgentsDifferentVillageMediumAffinity()) {
						a1->createAffinity(agentsIds[j], 1);
						a2->createAffinity(agentsIds[i], 1);
					}
					else {
						a1->createAffinity(agentsIds[j], 0);
						a2->createAffinity(agentsIds[i], 0);
					}
				}
			}
		}
	}
}

void MyWorld::initVillage(int id, int x, int y) {
	MyVillage v(id);
	Engine::Point2D<int> p;
	p._x = x;
	p._y = y;
	v.setLocation(p);
	_villages.push_back(v);
}

} // namespace Tutorial 
