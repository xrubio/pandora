
#include <HunterGatherer.hxx>
#include <GujaratWorld.hxx>
#include <Exception.hxx>
#include <Action.hxx>
#include <Sector.hxx>
#include <cmath>
#include <cassert>
#include <AgentController.hxx>

#include <GujaratState.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <typedefs.hxx>

namespace Gujarat
{

HunterGatherer::HunterGatherer( const std::string & id ) : GujaratAgent(id), _homeRange(50), _numSectors( -1 )
{
}

void HunterGatherer::registerAttributes()
{
	std::stringstream logName;
	logName << "Serializer_" << _world->getId();
	
	log_DEBUG(logName.str(), "registering attributes for type: " << getType());

	registerIntAttribute("MoveHome actions");
	registerIntAttribute("agent age");

	registerIntAttribute("children");
	registerIntAttribute("collected resources");
	registerIntAttribute("starving %");
	log_DEBUG(logName.str(), "registering attributes for type: " << getType() << " finished");
}

HunterGatherer::~HunterGatherer()
{
	for ( unsigned k = 0; k < _sectors.size(); k++ )
	{
		delete _sectors[k];
	}
}

void HunterGatherer::updateKnowledge( const Engine::Point2D<int>& agentPos, const Engine::DynamicRaster& dataRaster, std::vector<Sector*>& sectors ) const
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
			if ( !_world->getBoundaries().contains(p) )
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
			_sectors[k]->clearCells();
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
			if ( !_world->getBoundaries().contains(p) )
			{
				continue;
			}
			_sectors[indexSector]->addCell( p );
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
	std::list<MDPAction*> actions;
	GujaratState::controller().selectActions(*this, actions);
	std::list<MDPAction*>::iterator it=actions.begin();
	while(it!=actions.end())
	{
		_actions.push_back((Engine::Action*)(*it));
		it = actions.erase(it);
	}
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

	agent->setWalkingSpeedHour( _walkingSpeedHour );
	agent->setForageTimeCost( _forageTimeCost );
	agent->setMassToCaloriesRate( _massToCaloriesRate );
	agent->setNumSectors( _sectors.size() );
	
	// initially the agent will be a couple
	agent->_populationAges.resize(2);

	return agent;
}

void HunterGatherer::serialize()
{
	serializeAttribute("agent age", _age);
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
	serializeAttribute("starving %", (int)getPercentageOfStarvingDays());
	serializeAttribute("MoveHome actions", _moveHomeActionsExecuted);
}

} // namespace Gujarat

