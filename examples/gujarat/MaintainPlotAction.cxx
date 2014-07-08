
#include <MaintainPlotAction.hxx>

#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <AgroPastoralist.hxx>

#include <CultivatedField.hxx>

namespace Gujarat
{

MaintainPlotAction::MaintainPlotAction()
{
}

MaintainPlotAction::~MaintainPlotAction()
{
}

void MaintainPlotAction::execute( Engine::Agent & agent )
{
	AgroPastoralist & agroPastoralist = (AgroPastoralist&)agent;
	std::cout << "DEBUG: Agent " << agent.getId() << " executing Maintain action..." << std::endl;
	agroPastoralist.getCultivatedField().increasePotential();
}

int MaintainPlotAction::getTimeNeeded() const
{
	return 5;
}
} // namespace Gujarat

