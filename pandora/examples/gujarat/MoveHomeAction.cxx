
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

namespace Gujarat
{

MoveHomeAction::MoveHomeAction( const Engine::Point2D<int>& p ) 
	: _newHomeLoc( p )
{
}

MoveHomeAction::~MoveHomeAction()
{
}

MDPAction * MoveHomeAction::copy() const
{
	return new MoveHomeAction( _newHomeLoc );
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
	Engine::Point2D<int> boxSize(boxSizeX,boxSizeY);	
	Engine::Rectangle<int> unTrimmedHomeBox(boxOrigin,boxSize);
	Engine::Rectangle<int> homeBox;
	//TODO look out sectors, MPI regions, etc... Decide getBoundaries? or getOverlapBoundaries?
	// MRJ: As I understand it, this should be getOverlapBoundaries(), very much as it was before
	unTrimmedHomeBox.intersection(agent.getWorld()->getOverlapBoundaries(),homeBox);
	
	// Retrieve the areas that have intersection non zero with homeBox
	const GujaratWorld * world              = (GujaratWorld *)agent.getWorld();
	const SettlementAreas * settlementAreas = world->getSettlementAreas();
	std::vector<int> candidates(0);
	settlementAreas->intersectionFilter(homeBox,candidates);

	assert( !candidates.empty() );
	
	// Select Areas with maximum score.
	// Sort candidates following scores	
	std::make_heap(candidates.begin(),candidates.end(),Gujarat::compareSettlementAreas(settlementAreas));		
	
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
		for (index._x = intersection._origin._x; index._x < intersection._origin._x+intersection._size._x; index._x++)			
		{
			for (index._y = intersection._origin._y; index._y < intersection._origin._y+intersection._size._y; index._y++)			
			{
				log_DEBUG(logName.str(), "checking pos: " << index << " from origin: " << agentPos);

				if ((world->getValue("soils",index) == DUNE) 
					&& (ceil(agentPos.distance(index)) <= (double)agent.getHomeMobilityRange()))
				{
					countDunes++;
					dunes.push_back(index);
				}
			}
		}		

		if ( dunes.empty() ) 
		{
			continue;
		}
		uint32_t diceSelectOneRandomDune = Engine::GeneralState::statistics().getUniformDistValue(0, dunes.size()-1);
		actions.push_back( new MoveHomeAction( dunes[ diceSelectOneRandomDune ] ) );
	}
	assert( !actions.empty() );
	candidates.clear();
	log_DEBUG(logName.str(), "generate possible actions for pos: " << agentPos << " finished");
}

void MoveHomeAction::execute( Engine::Agent & agent )
{
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId() << "_actions";
	log_DEBUG(logName.str(), " executing MoveHome action"); 

	int prevHomeActivity = agent.getWorld()->getValue( "homeActivity", _newHomeLoc );
	agent.getWorld()->setValue( "homeActivity", _newHomeLoc, prevHomeActivity + 1 );
	agent.setPosition( _newHomeLoc );
}

void MoveHomeAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	//std::cout << "MOVE HOME" << std::endl;
	sp.setLocation( _newHomeLoc );
}

int MoveHomeAction::getTimeNeeded() const
{
	return 1;//issue: set it in configuration file
}

} // namespace Gujarat

