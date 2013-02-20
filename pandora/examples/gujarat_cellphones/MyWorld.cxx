#include <MyWorld.hxx>
#include <Simulation.hxx>
#include <MyAgent.hxx>

namespace Tutorial 
{

MyWorld::MyWorld(Engine::Simulation &simulation, Examples::MyWorldConfig &config) : World(simulation, 1, false, "./data/results.h5") {
	_agentsCounter = 0;
	_config = config;
}

MyWorld::~MyWorld() {
}

void MyWorld::createRasters() {
	registerDynamicRaster("resources", true);
	getDynamicRasterStr("resources").setInitValues(0, 10, 0);
	createVillages();
}

std::vector<std::string> MyWorld::getIdsExistingAgents() { //TODO FIX
	std::vector<std::string> ids;
	for (int i = 0; i < _agentsCounter; ++i) {
		std::ostringstream oss;
		oss << "MyAgent_" << i;
		MyAgent* a = (MyAgent*)getAgent(oss.str());
		if (a->exists()) ids.push_back(a->getId());
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

MyVillage MyWorld::getVillage(int id) {
	for (int i = 0; i < _villages.size(); ++i) {
		if (_villages[i].getId() == id) return _villages[i];
	}
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
			createAgent(Engine::GeneralState::statistics().getUniformDistValue(0,_config.getNumVillages()-1), true);
		}
	}
	initSocialNetwork();
}

} // namespace Tutorial 
