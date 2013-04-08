
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include "BaseAction.hxx"
#include <string>

namespace QuantumExperiment
{
class Forager;
class ForagerState;

class MoveAction : public BaseAction
{
	Engine::Point2D<int> _newPosition;
public:
	MoveAction( const Engine::Point2D<int> & newPosition);
	virtual ~MoveAction();

	void executeMDP( const Forager & agent, const ForagerState & state, ForagerState & stateNext ) const;
	void execute( Engine::Agent & agent );
	BaseAction * copy() const;
	float getStarvationCost() const;

	std::string describe() const;
};

} // namespace QuantumExperiment

#endif // __MoveHomeAction_hxx__

