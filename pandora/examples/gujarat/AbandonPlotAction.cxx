
#include <AbandonPlotAction.hxx>
#include <GujaratAgent.hxx>
#include <AgroPastoralist.hxx>

namespace Gujarat
{

AbandonPlotAction::AbandonPlotAction()
{
}

AbandonPlotAction::~AbandonPlotAction()
{
}

void AbandonPlotAction::execute( GujaratAgent & agent )
{
	AgroPastoralist & agroPastoralist = (AgroPastoralist&)agent;
	agroPastoralist.abandonPlot();
}

int AbandonPlotAction::getTimeNeeded()
{
	return 1;
}

} // namespace Gujarat

