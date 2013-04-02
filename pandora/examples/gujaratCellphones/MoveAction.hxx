
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <string>
#include <Point2D.hxx>

namespace Engine
{
	class Agent;
}

namespace GujaratCellphones
{
class Herder;
class HerderState;

class MoveAction : public Engine::Action
{
	Engine::Point2D<int> _newPosition;
public:
	MoveAction( const Engine::Point2D<int> & newPosition);
	virtual ~MoveAction();

	void executeMDP( const Herder & agent, const HerderState & state, HerderState & stateNext ) const;
	void execute( Engine::Agent & agent );
	MoveAction * copy() const;

	const Engine::Point2D<int> & getNewPosition() const;
	
	//friend std::ostream & operator<<( std::ostream & stream, MoveAction * action );
};

} // namespace GujaratCellphones

#endif // __MoveHomeAction_hxx__

