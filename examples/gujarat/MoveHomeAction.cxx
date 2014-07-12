
#include <MoveHomeAction.hxx>

#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <HunterGatherer.hxx>

#include <GujaratWorld.hxx>
#include <HunterGathererMDPState.hxx>
#include <algorithm>
#include <vector>

#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Sector.hxx>
#include <ForageAction.hxx>

namespace Gujarat
{

MoveHomeAction::MoveHomeAction( const Engine::Point2D<int>& p, Sector * sectorToForage, bool ownsSectorPointer ) : _newHomeLoc( p ), _forageAction(0)
{
	_forageAction = new ForageAction(sectorToForage, ownsSectorPointer);
	_forageAction->setFullPopulation(false);
}

MoveHomeAction::MoveHomeAction( const Engine::Point2D<int>& p, ForageAction * forageAction ) : _newHomeLoc( p ), _forageAction(forageAction)
{
}

MoveHomeAction::~MoveHomeAction()
{
	if(_forageAction)
	{
		delete _forageAction;
	}
}

MDPAction * MoveHomeAction::copy() const
{
	return new MoveHomeAction( _newHomeLoc, (ForageAction*)_forageAction->copy() );
}

std::string MoveHomeAction::describe() const
{
	std::stringstream logMove;
	logMove << "move_home( " << _newHomeLoc._x << ", " << _newHomeLoc._y << ")";
	return logMove.str();
}

void	MoveHomeAction::generatePossibleActions( const GujaratAgent& agent, std::vector< MoveHomeAction* >& actions )
{
	generatePossibleActions( agent, agent.getPosition(), actions );
}

void MoveHomeAction::generatePossibleActions( const GujaratAgent& agent,
						const Engine::Point2D<int>& agentPos, 
						std::vector< MoveHomeAction* >& actions )
{
	std::stringstream logName;
	logName << "agents_" << agent.getWorld()->getId() << "_" << agent.getId();

	log_DEBUG(logName.str(), "generate possible actions for pos: " << agentPos);

	// Put a BOX around the home range.	
	int boxOriginX = agentPos._x - agent.getHomeMobilityRange();
	int boxOriginY = agentPos._y - agent.getHomeMobilityRange();
	int boxSizeX = 2*agent.getHomeMobilityRange()+1;
	int boxSizeY = 2*agent.getHomeMobilityRange()+1;
	Engine::Point2D<int> boxOrigin(boxOriginX, boxOriginY);	
	Engine::Size<int> boxSize(boxSizeX,boxSizeY);	
	Engine::Rectangle<int> unTrimmedHomeBox(boxSize, boxOrigin);
	Engine::Rectangle<int> homeBox;

	unTrimmedHomeBox.intersection(agent.getWorld()->getBoundaries(),homeBox);
	
	// Retrieve the areas that have intersection non zero with homeBox
	const GujaratWorld * world              = (GujaratWorld *)agent.getWorld();
	const SettlementAreas * settlementAreas = world->getSettlementAreas();
	std::vector<int> candidates(0);
	settlementAreas->intersectionFilter(homeBox,candidates);

	assert( !candidates.empty() );
		
	// Select Areas with maximum score.
	// Sort candidates following scores	
	
	std::make_heap(candidates.begin(),candidates.end(),Gujarat::compareSettlementAreas(settlementAreas));
	//std::cout << "number candidates: " << candidates.size() << std::endl;

	for ( unsigned i = 0; i < candidates.size(); i++ )
	{
		Engine::Rectangle<int> selectedArea = settlementAreas->getAreaById(candidates[i]);
		log_DEBUG(logName.str(), "selected area: " << selectedArea << " from origin: " << agentPos);
		Engine::Rectangle<int> intersection;
		// MRJ: If the selected area is outside of the "homeBox" then why caring about it all?
		// ATM: above you will find settlementAreas->intersectionFilter method call. This ensures any
		// item inside candidates vector has intersection with homeBox. So the 'if' is not
		// needed, we just recompute the intersection to use it.
		homeBox.intersection(selectedArea, intersection);
		
		// Extract one random dune cell which is inside the homeRange and inside the selected area.
		
		// count dunes from candidate area "i", 'selectedArea' variable
		int countDunes = 0;
		Engine::Point2D<int> index;
		std::vector< Engine::Point2D<int> > dunes;
		for (index._x = intersection._origin._x; index._x < intersection._origin._x+intersection._size._width; index._x++)			
		{
			for (index._y = intersection._origin._y; index._y < intersection._origin._y+intersection._size._height; index._y++)			
			{
				log_DEBUG(logName.str(), "checking pos: " << index << " from origin: " << agentPos);

				if ((world->getValue(eSoils,index) == DUNE) && (ceil(agentPos.distance(index)) <= (double)agent.getHomeMobilityRange()))
				{
					countDunes++;
					dunes.push_back(index);
				}
			}
		}		

		if(dunes.empty()) 
		{
			continue;
		}
		uint32_t diceSelectOneRandomDune = Engine::GeneralState::statistics().getUniformDistValue(0, dunes.size()-1);

		Engine::Point2D<int> newPos = dunes.at(diceSelectOneRandomDune);
		const HunterGatherer & agentConcrete = dynamic_cast< const HunterGatherer & >( agent );
		for(std::vector<Sector *>::const_iterator it=agentConcrete.getSectors().begin(); it!=agentConcrete.getSectors().end(); it++)
		{
			if(!(*it)->isEmpty())
			{
				actions.push_back( new MoveHomeAction( newPos, *it ) );
			}
		}
	}

	assert( !actions.empty() );
	candidates.clear();
	log_DEBUG(logName.str(), "generate possible actions for pos: " << agentPos << " finished");
	//std::cout << "possible actions for MoveHome: " << actions.size() << std::endl;
}

void MoveHomeAction::execute( Engine::Agent & agent )
{
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId() << "_actions";
	log_DEBUG(logName.str(), " executing MoveHome action"); 

	/*
	std::stringstream logName2;
	logName2 << "actions_" << agent.getWorld()->getId();
	log_INFO(logName2.str(), "timestep: " << agent.getWorld()->getCurrentTimeStep() << " agent: " << agent << " executes MoveHome");
	*/

	//std::cout << agent << " executing move home from pos: " << agent.getPosition() << " to: " << _newHomeLoc << std::endl;
	if(_forageAction)
	{
		_forageAction->execute(agent);
	}

	//int prevHomeActivity = agent.getWorld()->getValue( "homeActivity", _newHomeLoc );
	//agent.getWorld()->setValue( "homeActivity", _newHomeLoc, prevHomeActivity + 1 );
	agent.setPosition( _newHomeLoc );

	GujaratAgent & gujaratAgent = (GujaratAgent&)agent;
	gujaratAgent.incrementMoveHomeActionsExecuted();
}

void MoveHomeAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	//std::cout << "MOVE HOME" << std::endl;
	_forageAction->executeMDP(agent, s, sp);
	sp.setLocation( _newHomeLoc );
	//std::cout << "executeMDP- time index: " << s.getTimeIndex() << " MoveHomeAction from loc->resources: " << s.getLocation() << " -> " << s.getOnHandResources() << " to: " << sp.getLocation() << " -> " << sp.getOnHandResources() << " days starving: " << s.getDaysStarving() << std::endl;
}

int MoveHomeAction::getTimeNeeded() const
{
	return 1;//issue: set it in configuration file
}

int MoveHomeAction::getPotentialBiomass() const
{
	if(!_forageAction)
	{
		return 0;
	}
	return _forageAction->getSector().getBiomassAmount();
}


} // namespace Gujarat

