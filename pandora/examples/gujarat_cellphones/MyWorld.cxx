#include <MyWorld.hxx>

namespace GujaratCellphones
{

MyWorld::MyWorld(Engine::Simulation &simulation, MyWorldConfig &config) : World(simulation, 1, false, "./data/results.h5"), _climate(config, *this) {
	_agentsCounter = 0;
	_config = config;
	_dailyDrySeasonBiomassDecrease.resize(4);
	_dailyRainSeasonBiomassIncrease.resize(4);
	_remainingBiomass.resize(4);
	_remainingBiomass[3] = _remainingBiomass[2] = _remainingBiomass[1] = 0.0f;
	_yearlyBiomass.resize(4);
	generateDistancesMatrix(config.getSize());
	initCellSoils();
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
	int horizon = Engine::GeneralState::statistics().getUniformDistValue(2, _config.getHorizon());
	int width = Engine::GeneralState::statistics().getUniformDistValue(1, _config.getWidth());
	MyAgent* agent = new MyAgent(oss.str(), _config, this, initialAgent, _config.getAgentNeededResources(), _config.getAgentBirthResources());
	agent->configureMDP(horizon, width, _config.getExplorationBonus());
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
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);
	registerStaticRaster("soils", true, eSoils);
	//Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eSoils), "./resources/60/soils_60.tiff", this);
	// we need to keep track of resource fractions
	registerDynamicRaster("resourcesFraction", false, eResourcesFraction);
	getDynamicRaster(eResourcesFraction).setInitValues(0, std::numeric_limits<int>::max(), 0);
	if(_config.getBiomassDistribution().compare("linDecayFromWater")==0 or _config.getBiomassDistribution().compare("logDecayFromWater")==0) {
		registerStaticRaster("weightWater", false, eWeightWater);
		Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eWeightWater), "./resources/60/linWeight_water_60.tiff", this);
	}
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

float MyWorld::getBiomassVariation(bool wetSeason, Soils & cellSoil, const Engine::Point2D<int> & index) const {
	double variation = 0.0f;
	if(_config.getBiomassDistribution().compare("standard")==0) {
		if(wetSeason) variation = _dailyRainSeasonBiomassIncrease.at(cellSoil);
		else variation = -_dailyDrySeasonBiomassDecrease.at(cellSoil);
	}
	else if(_config.getBiomassDistribution().compare("linDecayFromWater")==0 or _config.getBiomassDistribution().compare("logDecayFromWater")==0) {
		variation = ((double)(_config.getSize()*_config.getSize())/((double)(_config.getSumWeights())))*getValue(eWeightWater, index);
		variation /= 10000.0f; //TODO validar
		if(wetSeason) variation *= (float)(_dailyRainSeasonBiomassIncrease.at(cellSoil)/10000.0f); //TODO validar	
		else variation *= -1.0f*(float)(_dailyDrySeasonBiomassDecrease.at(cellSoil)/10000.0f); //TODO validar
	}
	return variation;
}

Climate MyWorld::getClimate() {
	return _climate;
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

void MyWorld::initCellSoils() {
	_cellSoils = std::vector<std::vector<Soils> > (_config.getSize(), std::vector<Soils> (_config.getSize()));
	for (int i = 0; i < _config.getSize(); ++i) {
		for (int j = 0; j < _config.getSize(); ++j) {
			_cellSoils[i][j] = (Soils)(Engine::GeneralState::statistics().getUniformDistValue(2, 3));
		}
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

void MyWorld::recomputeYearlyBiomass() {
	// update all the map resources to the minimum of the year (in case it was diminished by agents)
	Engine::Point2D<int> index;
	for(index._x = _boundaries._origin._x; index._x < _boundaries._origin._x + _boundaries._size._x; index._x++ )                
	{
		for( index._y = _boundaries._origin._y; index._y < _boundaries._origin._y + _boundaries._size._y; index._y++ )
		{
			setValue(eResourcesFraction, index, 0);
			setValue(eResources, index, _remainingBiomass[getValue(eSoils, index)]);
			// 1. Compute factor between actual rain and average rain		
			float raininessFactor = _climate.getRain() / _climate.getMeanAnnualRain();
			double areaOfCell = _config.getCellResolution()*_config.getCellResolution(); 
			// 2. For each soil type compute yearly biomass	
			// data expressed in g/m2
			_yearlyBiomass[WATER] = 0.0f;
			_yearlyBiomass[DUNE] = areaOfCell*_config.getDuneBiomass() * raininessFactor * _config.getDuneEfficiency();
			_yearlyBiomass[INTERDUNE] = areaOfCell*_config.getInterduneBiomass() * _config.getInterduneEfficiency() * raininessFactor;

			// yearly biomass is the area of a triangle with max height at the end of wet season
			// A_1 + A_2 = biomass, being A_1 = daysPerSeason*h/2 and A_2 = 2*daysPerSeason*h/2
			// dPS*h/2 + 2*dPS*h/2 = biomass, so h = biomass/1.5*dPS
			// and A_2 = 2*A_1

			double heightInterDune = (_yearlyBiomass[INTERDUNE] + 120.0f*_config.getInterduneMinimum()-60.0*_remainingBiomass[INTERDUNE]) / (180+240*_config.getInterduneMinimum());

			_dailyRainSeasonBiomassIncrease[INTERDUNE] = (heightInterDune-_remainingBiomass[INTERDUNE])/120.0f;
			_remainingBiomass[INTERDUNE] = heightInterDune *_config.getInterduneMinimum();
			_dailyDrySeasonBiomassDecrease[INTERDUNE] = heightInterDune*(1-_config.getInterduneMinimum())/240.0f;

			double heightDune = (_yearlyBiomass[DUNE] + 120.0f*_config.getDuneMinimum()-60.0*_remainingBiomass[DUNE]) / (180+240*_config.getDuneMinimum());
			_dailyRainSeasonBiomassIncrease[DUNE] = (heightDune-_remainingBiomass[DUNE])/120.0f;
			_remainingBiomass[DUNE] = heightDune *_config.getDuneMinimum();
			_dailyDrySeasonBiomassDecrease[DUNE] = heightDune*(1-_config.getDuneMinimum())/240.0f;

			_dailyRainSeasonBiomassIncrease[WATER] = 0.0f;
			_dailyDrySeasonBiomassDecrease[WATER] = 0.0f;
			_remainingBiomass[WATER] = 0.0f;
		}
	}
}

void MyWorld::stepEnvironment() {
	_climate.advanceSeason();
	// if this is the first step of a wet season, rainfall and biomass are calculated for the entire year
	if (_climate.rainSeasonStarted()) {
		updateRainfall();
		recomputeYearlyBiomass();
	}
	// resources are updated each time step
	updateResources();
}

void MyWorld::updateRainfall() {
	_climate.step();
}

void MyWorld::updateResources() {
	Engine::Point2D<int> index;
	for( index._x=_boundaries._origin._x; index._x<_boundaries._origin._x+_boundaries._size._x; index._x++ )		
	{
		for( index._y=_boundaries._origin._y; index._y<_boundaries._origin._y+_boundaries._size._y; index._y++ )
		{
			// 3. Increment or Decrement cell biomass depending on yearly biomass
			//    figures and current timestep
			int currentValue = getValue(eResources, index);
			float currentFraction = (float)getValue(eResourcesFraction, index)/100.0f;
			//Soils cellSoil = (Soils)getValue(eSoils, index); //TODO cambiar esto
			//Soils cellSoil = (Soils)(Engine::GeneralState::statistics().getUniformDistValue(1, 3));
			Soils cellSoil = _cellSoils[index._x][index._y];
			if(cellSoil!=WATER)
			{
				Seasons season = _climate.getSeason();
				bool wetSeason = false;
				if(season==HOTWET)
				{
					wetSeason = true;
				}			
				float newValue = std::max(0.0f, currentValue+currentFraction+getBiomassVariation(wetSeason, cellSoil, index));
				currentValue = newValue;
				float fraction = 100.0f*(newValue  - currentValue);
				setValue(eResources, index, currentValue);
				setValue(eResourcesFraction, index, (int)fraction);
			}
		}
	}
}

}

