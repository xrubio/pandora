
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

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
	
class MoveHomeAction : public MDPAction
{
	Engine::Point2D<int>	_newHomeLoc;

public:
	MoveHomeAction( const Engine::Point2D<int>& p );
	virtual ~MoveHomeAction();
	// TODO templatitzar classe per agent
	void execute( Engine::Agent & agent );
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;

	int getTimeNeeded() const;

	static void	generatePossibleActions( const GujaratAgent& agent, std::vector< MoveHomeAction* >& actions );
	static void	generatePossibleActions( const GujaratAgent& agent, const Engine::Point2D<int>& agentPos, std::vector< MoveHomeAction* >& actions );

	virtual	MDPAction* copy() const;
	virtual std::string describe() const;
};
	
} // namespace Gujarat

#endif // __MoveHomeAction_hxx__

