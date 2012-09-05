
#ifndef __SowAction_hxx__
#define __SowAction_hxx__

#include <MDPAction.hxx>

namespace Engine
{
	class Agent;
}
namespace Gujarat
{

class GujaratAgent;
	
class SowAction : public MDPAction
{
public:
	SowAction( );
	virtual ~SowAction();
	// TODO templatitzar classe per agent
	void execute( Engine::Agent & agent );
	int getTimeNeeded() const;
};
	
} // namespace Gujarat

#endif // __SowAction_hxx__

