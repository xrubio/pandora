#include <MyWorld.hxx>
#include "MyAgent.hxx"

namespace GujaratCellphones
{

MyWorld::MyWorld(Engine::Simulation &simulation, MyWorldConfig &config) : World(simulation, 1, false, config._resultsFile), _climate(config)
{
	_agentsCounter = 0;
	_config = config;
	generateDistancesMatrix(config.getSize());

	_dailyDecrease.resize(11);
	_maxResources.resize(11);
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

MyAgent * MyWorld::createAgent(int idVillage, bool initialAgent)
{
	std::cout << "creating agent at village: " << idVillage << std::endl;
	std::ostringstream oss;
	oss << "MyAgent_" << _agentsCounter;
	MyAgent* agent = new MyAgent(oss.str(), _config, this, initialAgent, _config.getAgentNeededResources(), _config.getAgentBirthResources());
	agent->configureMDP(_config.getHorizon(), _config.getWidth(), _config.getExplorationBonus());
	addAgent(agent);
	_villages.at(idVillage).addCitizen(oss.str());
	agent->setVillage(_villages.at(idVillage));
	agent->initPosition();
	++_agentsCounter;
	return agent;
}

void MyWorld::createAgents()
{
	createVillages();
	for(int i = 0; i < _config.getNumAgents(); i++)
	{
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			createAgent(Engine::GeneralState::statistics().getUniformDistValue(0,_config.getNumVillages()-1), true);
		}
	}
	initSocialNetwork();
}

void MyWorld::createRasters()
{
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("soil quality", true, eSoilQuality);
	getDynamicRaster(eSoilQuality).setInitValues(0, 10, 5);

	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			int value = Engine::GeneralState::statistics().getNormalDistValue(0,10);
			getDynamicRaster(eSoilQuality).setMaxValue(index, value);
		}
	}
	updateRasterToMaxValues(eSoilQuality);	
	
	// we need to keep track of resource fractions
	registerDynamicRaster("resourcesFraction", false, eResourcesFraction);
	getDynamicRaster(eResourcesFraction).setInitValues(0, 100, 0);


}

void MyWorld::createVillages()
{
	initVillage(0, _config.getSize()/8, _config.getSize()/8);
	initVillage(1, 3*_config.getSize()/8, 3*_config.getSize()/8);
	initVillage(2, _config.getSize()/8, 5*_config.getSize()/8);
	initVillage(3, 3*_config.getSize()/8, 7*_config.getSize()/8);	
	initVillage(4, 5*_config.getSize()/8, _config.getSize()/8);
	initVillage(5, 7*_config.getSize()/8, 3*_config.getSize()/8);
	initVillage(6, 5*_config.getSize()/8, 5*_config.getSize()/8);
	initVillage(7, 7*_config.getSize()/8, 7*_config.getSize()/8);
}

void MyWorld::generateDistancesMatrix(int size) {
	_distancesMatrix = std::vector<std::vector<int> >(size*size, std::vector<int>(size*size, 0));
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			for (int i1 = 0; i1 < size; ++i1) {
				for (int j1 = 0; j1 < size; ++j1) {
					int d = 0;
					if (i == i1 and j == j1) d = 0;
					else if (i == i1) d = std::max(j, j1) - std::min(j, j1);
					else if (j == j1) d = std::max(i, i1) - std::min(i, i1);
					else {
						int difx = i - i1;
						if (difx < 0) difx *= -1;
						int dify = j - j1;
						if (dify < 0) dify *= -1;
						d = std::max(difx, dify);
					}
					_distancesMatrix[j*size + i][j1*size + i1] = d;
				}
			}
		}
	}
}

int MyWorld::getAgentsCounter() {
	return _agentsCounter;
}

int MyWorld::getDistance(int x1, int y1, int x2, int y2) {
	return _distancesMatrix[y1*_config.getSize() + x1][y2*_config.getSize() + x2];
}

std::vector<std::string> MyWorld::getIdsAgentsInCell(int x, int y) {
	std::vector<std::string> idsAgentsSameCell;
	std::vector<std::string> idsAgents = getIdsExistingAgents();
	for (int i = 0; i < idsAgents.size(); ++i) {
		MyAgent* a = (MyAgent*)getAgent(idsAgents[i]);
		if (a->getPosition()._x == x and a->getPosition()._y == y) {
			idsAgentsSameCell.push_back(a->getId());
		}
	}
	return idsAgentsSameCell;
}

std::vector<std::string> MyWorld::getIdsExistingAgents() {
	std::vector<std::string> ids;
	for (int i = 0; i < _agentsCounter; ++i) {
		std::ostringstream oss;
		oss << "MyAgent_" << i;
		if (not agentHasBeenDeleted(oss.str())) ids.push_back(oss.str());
	}
	return ids;
}

double MyWorld::getMaximumAvgCellsSharedPerCall() { 
	double max = 1;
	std::vector<std::string> ids = getIdsExistingAgents();
	for (int i = 0; i < ids.size(); ++i) {
		MyAgent* a = (MyAgent*)getAgent(ids[i]);
		if (a->getAvgCellsSharedPerCall() > max) {
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

void MyWorld::recomputeYearlyBiomass()
{
	float rainWeight = _climate.getRain()/_config._rainHistoricalDistribMean;
	std::cout << " rain: " << _climate.getRain() << " mean: " << _config._rainHistoricalDistribMean << " weight: " << rainWeight << std::endl;
	Engine::Point2D<int> index;

	for(int i=0; i<_maxResources.size(); i++)
	{
		float maxValue = i*rainWeight*10000.0f/5.0f;
		_maxResources.at(i) = maxValue;
		float decrease = maxValue*1.0f;
		// days of dry season -1 because the first day it is not decreased
		_dailyDecrease.at(i) = decrease/(_config._daysDrySeason-1);

	}

	for(index._x = _overlapBoundaries._origin._x; index._x < _overlapBoundaries._origin._x + _overlapBoundaries._size._x; index._x++ )                
	{
		for( index._y = _overlapBoundaries._origin._y; index._y < _overlapBoundaries._origin._y + _overlapBoundaries._size._y; index._y++ )
		{
			setValue(eResources, index, _maxResources.at(getValue(eSoilQuality, index)));
		}
	}
}

void MyWorld::stepEnvironment()
{
	// first day of year -> step to include whole wet season
	if(isWetSeason())
	{
		_climate.computeRainValue();
		recomputeYearlyBiomass();
	}
	else
	{
		updateResources();
	}
}

void MyWorld::updateResources()
{
	Engine::Point2D<int> index;
	for( index._x=_overlapBoundaries._origin._x; index._x<_overlapBoundaries._origin._x+_overlapBoundaries._size._x; index._x++ )		
	{
		for( index._y=_overlapBoundaries._origin._y; index._y<_overlapBoundaries._origin._y+_overlapBoundaries._size._y; index._y++ )
		{
			int currentValue = getValue(eResources, index);
			float decrease = _dailyDecrease.at(getValue(eSoilQuality, index));

			float currentFraction = (float)getValue(eResourcesFraction, index)/100.0f;
			float newValue = std::max(0.0f, currentValue+currentFraction-decrease);
			currentValue = newValue;
			float fraction = 100.0f*(newValue  - currentValue);

			setValue(eResources, index, currentValue);
			setValue(eResourcesFraction, index, (int)fraction);
		}
	}
}

bool MyWorld::isWetSeason() const
{
	if(_step%_config._daysDrySeason==0)
	{
		return true;
	}
	return false;
}

int MyWorld::daysUntilWetSeason() const
{
	int days = _step%_config._daysDrySeason;
	return _config._daysDrySeason - days;
}

}

