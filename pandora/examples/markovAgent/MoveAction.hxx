
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <string>
#include <Point2D.hxx>

namespace Engine
{
	class Agent;
}

namespace Examples
{
class MDPAgent;
class MDPAgentState;

class MoveAction : public Engine::Action
{
	Engine::Point2D<int> _newPosition;
	int _resourcesToCollect;
public:
	MoveAction( const Engine::Point2D<int> & newPosition, int resourcesToCollect );
	virtual ~MoveAction();

	void executeMDP( const MDPAgent & agent, const MDPAgentState & state, MDPAgentState & stateNext ) const;
	void execute( Engine::Agent & agent );
	MoveAction * copy() const;

	const Engine::Point2D<int> & getNewPosition() const;
	int getResourcesToCollect() const;
	
	//friend std::ostream & operator<<( std::ostream & stream, MoveAction * action );
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

