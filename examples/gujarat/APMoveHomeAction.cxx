
#include <APMoveHomeAction.hxx>

#include <GujaratWorld.hxx>
#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <AgroPastoralist.hxx>

namespace Gujarat
{

APMoveHomeAction::APMoveHomeAction( Engine::Point2D<int>& p ) 
	: _newHomeLoc( p )
{
}

APMoveHomeAction::~APMoveHomeAction()
{
}

void APMoveHomeAction::generatePossibleActions( GujaratAgent& agent,
						std::vector< APMoveHomeAction* >& actions )
{
	// TODO this method excludes the creation of children in cells in other environments
	Engine::Point2D<int> location;
	Engine::Point2D<int> currPos = agent.getPosition();
	int range = agent.getHomeMobilityRange();

	for( location._x = currPos._x-range; 
		location._x <= currPos._x+range; location._x++)
	{
		for( location._y = currPos._y-range; 
			location._y <= currPos._y+range; location._y++)
		{
			if(	agent.getWorld()->getOverlapBoundaries().isInside(location) && 
				agent.getWorld()->checkPosition(location) && 
				agent.getWorld()->getValue("soils", location)==DUNE)
			{
				actions.push_back( new APMoveHomeAction(location) );
			}
		}
	}
}

void APMoveHomeAction::execute( Engine::Agent & agent )
{
	AgroPastoralist & agroPastoralist = (AgroPastoralist&)agent;

	std::cout << "DEBUG: Agent " << agent.getId() << " executing Move Home action..." << std::endl;

	int prevHomeActivity = agent.getWorld()->getValue( "homeActivity", _newHomeLoc );
	agent.getWorld()->setValue( "homeActivity", _newHomeLoc, prevHomeActivity + 1 );
	agent.setPosition( _newHomeLoc );
	if ( agroPastoralist.hasCultivatedField() && agroPastoralist.cultivatedFieldOutOfReach() )
		agroPastoralist.abandonCultivatedField();
	
}

int APMoveHomeAction::getTimeNeeded() const
{
	return 5;
}

} // namespace Gujarat

