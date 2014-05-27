
#include <HunterGatherer.hxx>
#include <GujaratWorld.hxx>
#include <Exceptions.hxx>
#include <Action.hxx>
#include <Sector.hxx>
#include <cmath>
#include <cassert>
#include <AgentController.hxx>

#include <GujaratState.hxx>
#include <GeneralState.hxx>
#include <HGMindFactory.hxx>
#include <Logger.hxx>

#include <GujaratConfig.hxx>

#include <HGMindFactory.hxx>

#include <MDPAction.hxx>

#include <sstream>

namespace Gujarat
{

HunterGatherer::HunterGatherer( const std::string & id ) 
	: GujaratAgent(id)/*, _surplusForReproductionThreshold(2), _surplusWanted(1)*/ ,_homeRange(50)
	,_numSectors( -1 )
	
{
	omp_init_lock(&_mapLock);
}

void HunterGatherer::createMind()
{
	_myHGMind = (HGMindFactory::getInstance().getHGMind(*(GujaratWorld*)_world));
	
	_myHGMind->_logName << "logMDPStates_"	<< getWorld()->getId() << "_" << getId();
}

void HunterGatherer::registerAttributes()
{
	std::stringstream logName;
	logName << "Serializer_" << _world->getId();
	
	log_DEBUG(logName.str(), "registering attributes for type: " << getType());

	registerIntAttribute("MoveHome actions");
	registerIntAttribute("Forage actions");
	registerIntAttribute("agent age");
//	registerIntAttribute("male alive");
//	registerIntAttribute("male age");
//	registerIntAttribute("female alive");
//	registerIntAttribute("female age");
	registerIntAttribute("children");
	registerIntAttribute("collected resources");
	registerIntAttribute("resource prediction");
	registerIntAttribute("resource prediction ratio");
//	registerIntAttribute("starving %");
	registerIntAttribute("starving days x 100");
	log_DEBUG(logName.str(), "registering attributes for type: " << getType() << " finished");
}

HunterGatherer::~HunterGatherer()
{
	_myHGMind->clearSectorKnowledge();	
	delete _myHGMind;
	//omp_destroy_lock(_mapLock);
	//delete _mapLock;
	_objectUseCounter.clear();
}

//************************************************************************
// REFACTORED updateKnowledge & updateKnowledge(par1,par2...)const
//************************************************************************
// some slight changes in the methods allows to disable omniscience i
// perform update knowledge after exploring a sector

									   
void HunterGatherer::updateKnowledge()
{	
	_collectedResources = 0;	
	
	std::stringstream logName;
	logName << "agents_" << _world->getId() << "_" << getId();
	
	if(_myHGMind->_logName.str().size() == 0)
	{
		_myHGMind->_logName<< "logMDPStates_"	<< getWorld()->getId() << "_" << getId();	 //= logName;
	}
	
	_myHGMind->updateKnowledge(_position);

	//_world->setValue(eCounterHRSectors,_position,getHRSectors().size());
	//_world->setValue(eCounterLRSectors,_position,getLRSectors().size());
}


void	HunterGatherer::updateKnowledge( const Engine::Point2D<int>& agentPos
										, const Engine::Raster& dataRaster
										, std::vector< Sector* >* HRSectors
										, std::vector< Sector* >* LRSectors
										, std::vector< Engine::Point2D<int> >* HRCellPool
										, std::vector< Engine::Point2D<int> >* LRCellPool
									   ) const
{	
	std::stringstream logName;
	//logName << "logMDPStates_"	<< getWorld()->getId() << "_" << getId();	
	logName << "logMDPStates_"	<< getWorld()->getId() << "_" << getId();
	
	if(_myHGMind->_logName.str().size() == 0)
	{
		_myHGMind->_logName << "logMDPStates_"	<< getWorld()->getId() << "_" << getId();	// = logName;
	}
	
	assert(LRSectors->size()==0);
	
	_myHGMind->updateKnowledge(agentPos, dataRaster
								, HRSectors, LRSectors
								, HRCellPool, LRCellPool);
	
	//log_INFO(logName.str(),"after HGMind::UPDATE; HRSectors at " << agentPos 
		//				<< ", amount sectors " << HRSectors->size());		
	
	//log_INFO(logName.str(),"after HGMind::UPDATE; LRSectors at " << agentPos 
		//				<< ", amount sectors " << LRSectors->size());		
	
	if(_world->getValue(eCounterHRSectors,agentPos)==-1)
	{
		_world->setValue(eCounterHRSectors,agentPos,HRSectors->size());
		//log_INFO(logName.str(),"after HGMind::UPDATE; INIT HRSectors at " << agentPos << ", amount sectors " << HRSectors->size());	
	
	}
	else
	{
		if(_world->getValue(eCounterHRSectors,agentPos)!=HRSectors->size()) 
		{
			std::stringstream oss;
			oss << "HunterGatherer::updateKnowledge: position " << agentPos << ", raster says : " 
			<< _world->getValue(eCounterHRSectors,agentPos)
			<< " in the past, but now produces " << HRSectors->size() << " HR sectors";			
			
			//std::cout << oss.str() << std::endl;
			//log_INFO(logName.str(),oss.str());
			//throw Engine::Exception(oss.str());
			
		}
	}
	
	if(_world->getValue(eCounterLRSectors,agentPos)==-1) 
		_world->setValue(eCounterLRSectors,agentPos,LRSectors->size());
	else
	{
		if(_world->getValue(eCounterLRSectors,agentPos)!=LRSectors->size()) 
		{
			std::stringstream oss;
			oss << "HunterGatherer::updateKnowledge("<< (unsigned long)_myHGMind <<"): position " << agentPos << ", raster says : " 
			<< _world->getValue(eCounterLRSectors,agentPos)
			<< " in the past, but now produces " << LRSectors->size() << " LR sectors";
			
			//std::cout << oss.str() << std::endl;
			//throw Engine::Exception(oss.str());
			//log_INFO(logName.str(),oss.str());
		}
	}
	
}


void HunterGatherer::executeActions()
{
	
	std::list<Engine::Action *>::iterator it = _actions.begin();
	while(it!=_actions.end())
	{
		//Engine::Action * nextAction = _actions[i];
		MDPAction * nextAction = (MDPAction*)*it;
		// world info retrieved due to execute an action
		_myHGMind->updateDueToExecuteAction(((MDPAction*)nextAction)->getVisitedSector());
		it++;
	}
	// Ensure that no action alters the internal knowledge retrieved
	// by knowledgeDueToExecuteAction
	// MoveHomeAction : 
	//		erases sectors : HR, LR
	//		does not touch timestamps
	//		does not touch private rasters
	// ForageAction :
	//		touches nothing
	
	GujaratAgent::executeActions();

	if(((GujaratConfig)((GujaratWorld*)_world)->getConfig())._shareInfo) 
	{
		doInformationSharing();
	}
}

void HunterGatherer::clearSectorKnowledge() 
	{ 
		_myHGMind->clearSectorKnowledge();
	}	
	
void HunterGatherer::selectActions()
{
	//*?
	//std::stringstream logName;
	//logName << "infoshar";
	
	std::list<MDPAction*> actions;
	
/*	
	GujaratState::controller().selectActions(*this, actions);
	
	assert(actions.size() > 0);
	//*?
	std::list<MDPAction*>::iterator it=actions.begin();
	while(it!=actions.end())
	{
		_actions.push_back((Engine::Action*)(*it));
		//it = actions.erase(it);
		it++;
	}
	
	
	try{
		std::list<MDPAction*>::iterator it=actions.begin();
		while(it!=actions.end())
		{
			//_actions.push_back((Engine::Action*)(*it));
			it = actions.erase(it);
		}	
	} catch (std::exception & exceptionThrown) 
		{
			std::cout << exceptionThrown.what() << std::endl;
			assert(0==1);
		}
*/
	//*?
	try{ int w = GujaratState::controller().getWidth();}
	catch(std::exception e){std::cerr << "EXC getWidth():"<<e.what()<<std::endl;}	


	try{
		GujaratState::controller().selectActions(*this, _actions);
	}catch(std::exception e){std::cerr << "EXC selectActions:"<<e.what()<<std::endl;}

	
}

GujaratAgent * HunterGatherer::createNewAgent()
{	
	GujaratWorld * world = (GujaratWorld*)_world;
	std::ostringstream oss;
	oss << "HunterGatherer_" << world->getId() << "-" << world->getNewKey();

	HunterGatherer * agent = new HunterGatherer(oss.str());

	agent->_world = _world;
	agent->setSocialRange( _socialRange );
	agent->setHomeMobilityRange( _homeMobilityRange );
	agent->setHomeRange( _homeRange );
	agent->setLowResHomeRange( _lowResHomeRange );
	
	//agent->setSurplusForReproductionThreshold( _surplusForReproductionThreshold );
	//agent->setSurplusWanted( _surplusWanted );
	//agent->setSurplusSpoilageFactor( _surplusSpoilageFactor );
	//agent->setFoodNeedsForReproduction( _foodNeedsForReproduction );			

	agent->setWalkingSpeedHour( _walkingSpeedHour );
	agent->setForageTimeCost( _forageTimeCost );
	//agent->setAvailableForageTime( _availableForageTime );
	agent->setMassToCaloriesRate( _massToCaloriesRate );
	agent->setNumSectors( ((GujaratConfig)((GujaratWorld*)_world)->getConfig())._numSectors );
	
	agent->createMind();
	
	// initially the agent will be a couple
	agent->_populationAges.resize(2);

	return agent;
	
}

/*
bool HunterGatherer::needsResources()
{
	return _collectedResources < (_surplusForReproductionThreshold + _surplusWanted);
}
*/

Engine::Raster & HunterGatherer::getLRResourcesRaster() 
{ 
	//return _world->getDynamicRaster(eLRResources); 	
	return _myHGMind->getLRResourcesRaster();
}

bool HunterGatherer::cellValid( Engine::Point2D<int>& loc )
{
	if ( !_world->getOverlapBoundaries().isInside(loc) )
		return false;
	// Check that the home of another agent resides in loc
	std::vector<Agent * > agents = _world->getAgent(loc);
	if(agents.size()==0)
	{
		agents.clear();
		return true;
	}

	for(unsigned int i=0; i<agents.size(); i++)
	{
		Agent * agent = agents.at(i);
		if(agent->exists() && agent!=this)
		{
			agents.clear();
			return false;
		}
	}
	agents.clear();
	return true;
}

bool HunterGatherer::cellRelevant( Engine::Point2D<int>& loc )
{
	Soils soilType = (Soils) _world->getValue(eSoils, loc);
	int resourceType = _world->getValue(eResourceType, loc);
	return soilType == INTERDUNE && resourceType == WILD;
}

void HunterGatherer::serialize()
{
	serializeAttribute("agent age", _age);
/*
	if(_populationAges[0]!=-1)
	{
		serializeAttribute("male alive", 1);
		serializeAttribute("male age", _populationAges[0]);
	}
	else
	{
		serializeAttribute("male alive", 0);
		serializeAttribute("male age", std::numeric_limits<int>::max());
	}
	
	if(_populationAges[1]!=-1)
	{
		serializeAttribute("female alive", 1);
		serializeAttribute("female age", _populationAges[1]);
	}
	else
	{
		serializeAttribute("female alive", 0);
		serializeAttribute("female age", std::numeric_limits<int>::max());
	}
*/
	int numChildren = 0;
	for(unsigned i=2; i<_populationAges.size(); i++)
	{
		if(_populationAges[i]!=-1)
		{
			numChildren++;
		}
	}
	serializeAttribute("children", numChildren);
	serializeAttribute("collected resources", _collectedResources);
	serializeAttribute("resource prediction", _resourcePrediction);
	serializeAttribute("resource prediction ratio", _resourcePrediction/(1.0f+(float)_collectedResources));
//	serializeAttribute("starving %", getPercentageOfStarvingDays());
	serializeAttribute("starving days x 100", _starved*100.0f);
	serializeAttribute("MoveHome actions", _moveHomeActionsExecuted);
	serializeAttribute("Forage actions", _forageActionsExecuted);
}


//********************** SHARE INFORMATION METHODS
void HunterGatherer::doInformationSharing()
{	
	Engine::World::AgentsVector neighbours = _world->getNeighbours(this, _socialRange, getType());
	for(Engine::World::AgentsVector::iterator it=neighbours.begin(); it!=neighbours.end(); it++)
	{
		HunterGatherer * a = (HunterGatherer*)(*it);
		if(getId() > a->getId())
		{
			shareInformation(a);
			//Issue: If I would decide to share depending on utility of sector, 
			// after each sharing my sectors should have their utility
			// updated.
		}
	}
	
}


void HunterGatherer::shareInformation( HunterGatherer * a)
{
	
	// select one random sector s
	int s = Engine::GeneralState::statistics().getUniformDistValue(0,_myHGMind->getLRSectors().size()-1);
	
	a->putInformation(_myHGMind->getLRSectors()[s]
					,_myHGMind->getLRResourcesRaster()
					,_myHGMind->getLRTimeStamps());
	
	a->shareInformation(this);
}


void HunterGatherer::putInformation(Sector *s, const Engine::Raster & r, const Engine::Raster & t)
{
	const 	std::vector< Engine::Point2D<int>* > & cells = s->cells();
	
	Engine::Raster & myLRTimeRaster = _myHGMind->getLRTimeStamps();
	Engine::Raster & myLRResRaster = _myHGMind->getLRResourcesRaster();
	
	for(unsigned int i = 0; i < cells.size(); i++)
	{	
		/*
		int ts_i = ((GujaratWorld*)_world)->getValueLR(t,cells[i]);
		//check timestamp!!! do not take information older than yours
		if (ts_i > ((GujaratWorld*)_world)->getValueLR(_myLRTimeRaster,cells[i]))
		{
			int u  = ((GujaratWorld*)_world)->getValueLR(r,cells[i]);
			((GujaratWorld*)_world)->setValueLR(_myLRResRaster,cells[i],u);
			((GujaratWorld*)_world)->setValueLR(_myLRTimeRaster,cells[i],ts_i);
		}
		*/
		int ts_i = t.getValue(*cells[i]);
		//check timestamp!!! do not take information older than yours
		if (ts_i > myLRTimeRaster.getValue(*cells[i]) 
			&& 
			ts_i - myLRTimeRaster.getValue(*cells[i]) > 3 )
		{
			int u  = r.getValue(*cells[i]);
			myLRResRaster.setInitValue(*cells[i],u);
			myLRTimeRaster.setInitValue(*cells[i],ts_i);
		}
	}
	
}

//*************************************************************






} // namespace Gujarat

