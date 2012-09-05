
#include <SowAction.hxx>

#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <AgroPastoralist.hxx>

#include <CultivatedField.hxx>

namespace Gujarat
{

SowAction::SowAction()
{
}

SowAction::~SowAction()
{
}

void SowAction::execute( Engine::Agent & agent )
{
	AgroPastoralist & agroPastoralist = (AgroPastoralist&)agent;
	if ( !agroPastoralist.getCultivatedField().isDomesticated() )
		return;
	agroPastoralist.getCultivatedField().sow();
 	std::cout << "DEBUG: Agent " << agent.getId() << " executing Sow action..." << std::endl;
}

int SowAction::getTimeNeeded() const
{
	return 5;
}
} // namespace Gujarat

