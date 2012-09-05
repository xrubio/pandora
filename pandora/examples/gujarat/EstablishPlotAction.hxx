
#ifndef __EstablishPlotAction_hxx__
#define __EstablishPlotAction_hxx__

#include <MDPAction.hxx>
#include <Point2D.hxx>
#include <vector>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{

class GujaratAgent;
	
class EstablishPlotAction : public MDPAction
{
public:
	EstablishPlotAction( Engine::Point2D<int> p );
	virtual ~EstablishPlotAction();

	void execute( Engine::Agent & agent );
	int getTimeNeeded() const;

	static void generatePossibleActions( GujaratAgent& agent,
	std::vector< EstablishPlotAction* >& actions );

private:
	
	Engine::Point2D<int>	_plotLocation;
};
	
} // namespace Gujarat

#endif // __EstablishPlotAction_hxx__

