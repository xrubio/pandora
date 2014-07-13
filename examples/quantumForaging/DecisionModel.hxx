
#ifndef __DecisionModel_hxx__
#define __DecisionModel_hxx__

#include <engine/problem.h>

namespace QuantumExperiment
{
class Forager;
class ForagerState;

typedef std::vector<std::pair<ForagerState,float> > OutcomeVector; 

class DecisionModel : public Problem::problem_t<ForagerState>
{
public:
	DecisionModel( Forager & agent );
	virtual ~DecisionModel();

	void reset();

	// Interface inherited from Problem::problem_t<T>
	virtual Problem::action_t number_actions( const ForagerState &s ) const;
	virtual const ForagerState & init() const;
	virtual bool terminal( const ForagerState & state ) const;
	virtual bool dead_end( const ForagerState & state ) const;
	virtual bool applicable( const ForagerState & state, Problem::action_t action ) const;
	virtual float cost( const ForagerState &state, Problem::action_t action ) const;
	virtual void next( const ForagerState & state, Problem::action_t action, OutcomeVector & outcomes ) const;
	
	virtual	void print( std::ostream& os ) const {}
protected:
	void makeActionsForState( ForagerState & state ) const;
	void applyFrameEffects( const ForagerState& state,  ForagerState& stateP ) const;

	Forager & _agent;
	ForagerState * _initial;
};

} // namespace QuantumExperiment

#endif // __DecisionModel_hxx__

