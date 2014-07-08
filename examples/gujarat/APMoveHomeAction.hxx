
#ifndef __APMoveHomeAction_hxx__
#define __APMoveHomeAction_hxx__

#include <vector>

#include <MDPAction.hxx>
#include <Point2D.hxx>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{

class GujaratAgent;
	
class APMoveHomeAction : public MDPAction
{
	Engine::Point2D<int>	_newHomeLoc;

public:
	APMoveHomeAction( Engine::Point2D<int>& p );
	virtual ~APMoveHomeAction();
	void execute( Engine::Agent & agent );
	virtual int getTimeNeeded() const;

	static void	generatePossibleActions( GujaratAgent& agent, std::vector< APMoveHomeAction* >& actions );
};
	
} // namespace Gujarat

#endif // __APMoveHomeAction_hxx__

