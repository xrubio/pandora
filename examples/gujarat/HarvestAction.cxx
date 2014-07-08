
#include <HarvestAction.hxx>

#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <AgroPastoralist.hxx>

#include <CultivatedField.hxx>

namespace Gujarat
{

HarvestAction::HarvestAction()
{
}

HarvestAction::~HarvestAction()
{
}

void HarvestAction::execute( Engine::Agent & agent )
{
	AgroPastoralist & agroPastoralist = (AgroPastoralist&)agent;

	if( !agroPastoralist.getCultivatedField().isSown() )
		return;
	agroPastoralist.acquireResources( agroPastoralist.getCultivatedField().harvest() );

	std::cout << "DEBUG: Agent " << agent.getId() << " executing Harvest action..." << std::endl;
}

int HarvestAction::getTimeNeeded() const
{
	return 5;
}
} // namespace Gujarat

