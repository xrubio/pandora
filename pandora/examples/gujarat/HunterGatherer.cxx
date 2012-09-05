
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
#include <Logger.hxx>

namespace Gujarat
{

HunterGatherer::HunterGatherer( const std::string & id ) 
	: GujaratAgent(id)/*, _surplusForReproductionThreshold(2), _surplusWanted(1)*/, _homeRange(50),
	_numSectors( -1 )
{
}

void HunterGatherer::registerAttributes()
{
	std::stringstream logName;
	logName << "Serializer_" << _world->getId();
	
	log_DEBUG(logName.str(), "registering attributes for type: " << getType());

	registerIntAttribute("agent age");
	registerIntAttribute("male alive");
	registerIntAttribute("male age");
	registerIntAttribute("female alive");
	registerIntAttribute("female age");
	registerIntAttribute("children");
	registerIntAttribute("collected resources");
	registerIntAttribute("starving");
	log_DEBUG(logName.str(), "registering attributes for type: " << getType() << " finished");
}

HunterGatherer::~HunterGatherer()
{
	for ( unsigned k = 0; k < _sectors.size(); k++ )
	{
		delete _sectors[k];
	}
}

void HunterGatherer::updateKnowledge( const Engine::Point2D<int>& agentPos, const Engine::Raster& dataRaster, std::vector<Sector*>& sectors ) const
{
	for ( unsigned k = 0; k < _numSectors; k++ )
	{
		sectors.push_back( new Sector(*getWorld()) );
	}

	for ( int x=-_homeRange; x<=_homeRange; x++ )
	{
		for ( int y=-_homeRange; y<=_homeRange; y++ )
		{
			int indexSector = GujaratState::sectorsMask(x+_homeRange,y+_homeRange);
			if ( indexSector == - 1 )
			{
				continue;
			}

			Engine::Point2D<int> p;
			p._x = agentPos._x + x;
			p._y = agentPos._y + y;
			if ( !_world->getOverlapBoundaries().isInside(p) )
			{
				continue;
			}
			sectors[indexSector]->addCell( p );
		}
	}

	for ( unsigned k = 0; k < _numSectors; k++ )
	{
		sectors[k]->updateFeatures(dataRaster);
	}
}

void HunterGatherer::updateKnowledge()
{
	// H/G can't preserve resources
	//std::cout << "collected from last time: " << _collectedResources << " surplus: " << _collectedResources - computeConsumedResources(1);
	//_collectedResources -= computeConsumedResources(1);
	//_collectedResources *= getSurplusSpoilageFactor();

	//std::cout << " spoiled: " << _collectedResources << " needed resources: " << computeConsumedResources(1) <<  std::endl;
	_collectedResources = 0;
	std::stringstream logName;
	logName << "agents_" << _world->getId() << "_" << getId();

	log_DEBUG(logName.str(), "update knowledge");
	// sectors not initialized
	log_DEBUG(logName.str(), "new sectors");
	if(_sectors.size()==0)
	{
		_sectors.resize(_numSectors);
		for ( unsigned k = 0; k < _numSectors; k++ )
		{
			_sectors[k] = new Sector( getWorldRef());
		}
	}
	else
	{
		for ( unsigned k = 0; k < _numSectors; k++ )
		{
			//std::cout << this << "clearing sector: " << k << std::endl;
			_sectors[k]->clearCells();
			//std::cout << "DONE!" <<  std::endl;
		}
	}
	
	log_DEBUG(logName.str(), "create sectors with home range: " << _homeRange);
	for ( int x=-_homeRange; x<=_homeRange; x++ )
	{
		for ( int y=-_homeRange; y<=_homeRange; y++ )
		{
			int indexSector = GujaratState::sectorsMask(x+_homeRange,y+_homeRange);
			if ( indexSector == - 1 )
			{
				continue;
			}

			Engine::Point2D<int> p;
			p._x = _position._x + x;
			p._y = _position._y + y;
			// TODO overlapboundaries
			if ( !_world->getBoundaries().isInside(p) )
			{
				continue;
			}
			_sectors[indexSector]->addCell( p );
			getWorld()->setValue( "sectors", p, 1 );
		}
	}
	log_DEBUG(logName.str(), "update features");

	for ( unsigned k = 0; k < _numSectors; k++ )
	{
		//std::cout << "Sector #" << (k+1) << " features:" << std::endl;
		_sectors[k]->updateFeatures();
		//_sectors[k]->showFeatures( std::cout );
	}
	log_DEBUG(logName.str(), "end update knowledge");
}

void HunterGatherer::selectActions()
{
	_actions.push_back( (Engine::Action*)(GujaratState::controller().selectAction(*this)) );
}

GujaratAgent * HunterGatherer::createNewAgent()
{	
	GujaratWorld * world = (GujaratWorld*)_world;
	std::ostringstream oss;
	oss << "HunterGatherer_" << world->getId() << "-" << world->getNewKey();
	
	HunterGatherer * agent = new HunterGatherer(oss.str());

	agent->setSocialRange( _socialRange );
	agent->setHomeMobilityRange( _homeMobilityRange );
	agent->setHomeRange( _homeRange );
	//agent->setSurplusForReproductionThreshold( _surplusForReproductionThreshold );
	//agent->setSurplusWanted( _surplusWanted );
	//agent->setSurplusSpoilageFactor( _surplusSpoilageFactor );
	//agent->setFoodNeedsForReproduction( _foodNeedsForReproduction );			

	agent->setWalkingSpeedHour( _walkingSpeedHour );
	agent->setForageTimeCost( _forageTimeCost );
	agent->setAvailableForageTime( _availableForageTime );
	agent->setMassToCaloriesRate( _massToCaloriesRate );
	agent->setNumSectors( _sectors.size() );
	
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

bool HunterGatherer::cellValid( Engine::Point2D<int>& loc )
{
	if ( !_world->getOverlapBoundaries().isInside(loc) )
		return false;
	// Check that the home of another agent resides in loc
	std::vector<Agent * > agents = _world->getAgent(loc);
	if(agents.size()==0)
	{
		return true;
	}

	for(int i=0; i<agents.size(); i++)
	{
		Agent * agent = agents.at(i);
		if(agent->exists() && agent!=this)
		{
			return false;
		}
	}
	return true;
}

bool HunterGatherer::cellRelevant( Engine::Point2D<int>& loc )
{
	Soils soilType = (Soils) _world->getValue("soils", loc);
	int resourceType = _world->getValue("resourceType", loc);
	return soilType == INTERDUNE && resourceType == WILD;
}

void HunterGatherer::serialize()
{
	serializeAttribute("agent age", _age);

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
	serializeAttribute("starving", getPercentageOfStarvingDays());
}

} // namespace Gujarat

