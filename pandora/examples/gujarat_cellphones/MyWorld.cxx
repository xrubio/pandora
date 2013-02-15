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

double MyWorld::getMaximumAvgCellsSharedPerCall() { 
	/*double max = -1;
	for (int i = 0; i < _agentsCounter; ++i) {
		std::ostringstream oss;
		oss << "MyAgent_" << i;
		MyAgent a = getAgent(oss.str());
		if (a.exists() and (max == -1 or a.getAvgCellsSharedPerCall() > max)) {
			max = a.getAvgCellsSharedPerCall();
		}
	}		
	return max;*/
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

std::string MyWorld::createAgent(int idVillage) {
	std::ostringstream oss;
	oss << "MyAgent_" << _agentsCounter;
	MyAgent* agent = new MyAgent(oss.str(), _config, this);
	addAgent(agent);
	agent->setVillage(_villages[idVillage]);
	agent->initPosition();
	++_agentsCounter;
	return oss.str();
}

void MyWorld::createAgents() {
	for(int i = 0; i < _config.getNumAgents(); ++i) {
		if((i%_simulation.getNumTasks())==_simulation.getId()) {
			createAgent(Engine::GeneralState::statistics().getUniformDistValue(0,_config.getNumVillages()-1));
		}
	}
}

} // namespace Tutorial 
