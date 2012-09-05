#include <EstablishPlotAction.hxx>
#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <AgroPastoralist.hxx>
#include <GujaratWorld.hxx>

namespace Gujarat
{

EstablishPlotAction::EstablishPlotAction( Engine::Point2D<int> p )
	: _plotLocation(p)
{
}

EstablishPlotAction::~EstablishPlotAction()
{
}

void EstablishPlotAction::execute( Engine::Agent & agent )
{
	AgroPastoralist & agroPastoralist = (AgroPastoralist&)agent;
	agroPastoralist.acquireCultivatedField(_plotLocation);
	int oldValue = agroPastoralist.getWorld()->getValue( "farmingActivity", _plotLocation );
	agroPastoralist.getWorld()->setValue( "farmingActivity", _plotLocation, oldValue + 1 );
	std::cout << "DEBUG: Agent " << agent.getId() << " executing Establish Plot action..." << std::endl;
}

int EstablishPlotAction::getTimeNeeded() const
{
	return 5;
}

void EstablishPlotAction::generatePossibleActions( GujaratAgent& agentRef, std::vector< EstablishPlotAction* >& actions )
{
	AgroPastoralist & agent  = (AgroPastoralist&)agentRef;
	GujaratWorld* world = dynamic_cast<GujaratWorld* >( agent.getWorld() );
	Engine::Point2D<int> newPosition(-1,-1);
	Engine::Point2D<int> current = agent.getPosition();

	for( newPosition._x=current._x-agent.getMaxCropHomeDistance(); 
		newPosition._x<=current._x+agent.getMaxCropHomeDistance(); 
		newPosition._x++)
	{
		for(	newPosition._y = current._y - agent.getMaxCropHomeDistance(); 
			newPosition._y <= current._y + agent.getMaxCropHomeDistance(); 
			newPosition._y++)
		{
			// by now common home is excluded
			if( world->getOverlapBoundaries().isInside(newPosition) 
				&& world->checkPosition(newPosition))
			{
				if ( world->isInterdune( newPosition )
					&& world->isWild( newPosition ) )
					actions.push_back(new EstablishPlotAction( newPosition ) );
			}
		}
	}
	
}

} // namespace Gujarat

