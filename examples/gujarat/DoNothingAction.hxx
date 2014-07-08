#ifndef __DoNothingAction_hxx__
#define __DoNothingAction_hxx__

#include <MDPAction.hxx>
#include <vector>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{

class GujaratAgent;

class DoNothingAction : public MDPAction
{
public:
	DoNothingAction();
	virtual ~DoNothingAction();

	void execute( Engine::Agent& agent );
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;

	int	getTimeNeeded() const { return 0; }
	static	void	generatePossibleActions( GujaratAgent& agent, std::vector< DoNothingAction* >& actions );

	virtual MDPAction* copy() const;
	virtual std::string describe() const;
protected:
	
};

}

#endif // DoNothingAction.hxx
