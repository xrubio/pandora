
#ifndef __ForageHomeAction_hxx__
#define __ForageHomeAction_hxx__

#include "BaseAction.hxx"
#include <string>
#include <Point2D.hxx>

namespace QuantumExperiment
{
class Forager;
class ForagerState;

class ForageAction : public BaseAction
{
public:
	ForageAction( const Engine::Point2D<int> & position);
	virtual ~ForageAction();

	void executeMDP( const Forager & agent, const ForagerState & state, ForagerState & stateNext ) const;
	void execute( Engine::Agent & agent );
	BaseAction * copy() const;
	std::string describe() const;
};

} // namespace QuantumExperiment

#endif // __ForageHomeAction_hxx__

