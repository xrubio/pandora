
#ifndef __MaintainPlotAction_hxx__
#define __MaintainPlotAction_hxx__

#include <MDPAction.hxx>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{

class GujaratAgent;
	
class MaintainPlotAction : public MDPAction
{
public:
	MaintainPlotAction();
	virtual ~MaintainPlotAction();
	// TODO templatitzar classe per agent
	void execute( Engine::Agent & agent );
	int getTimeNeeded() const;
};
	
} // namespace Gujarat

#endif // __MaintainPlotAction_hxx__

