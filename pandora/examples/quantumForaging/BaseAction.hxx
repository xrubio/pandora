
#ifndef __BaseAction_hxx__
#define __BaseAction_hxx__

#include <Action.hxx>
#include <Point2D.hxx>

namespace QuantumExperiment
{

class Forager;
class ForagerState;

class BaseAction : public Engine::Action
{
protected:
	Engine::Point2D<int> _position;
	int _resources;
public:
	BaseAction(const Engine::Point2D<int> & position) : _position(position), _resources(0) {}
	virtual ~BaseAction() {}

	virtual void executeMDP( const Forager & agent, const ForagerState & state, ForagerState & stateNext ) const = 0;
	virtual BaseAction * copy() const = 0;
	int getResources() const { return _resources; }
	virtual float getStarvationCost() const = 0;
	const Engine::Point2D<int> & getPosition() const { return _position; }
};

} // namespace QuantumExperiment

#endif // __BaseAction_hxx__

