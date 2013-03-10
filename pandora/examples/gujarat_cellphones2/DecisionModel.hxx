
#ifndef __DecisionModel_hxx__
#define __DecisionModel_hxx__

#include <engine/problem.h>

namespace GujaratCellphones
{
class Herder;
class HerderState;

typedef std::vector<std::pair<HerderState,float> > OutcomeVector; 

class DecisionModel : public Problem::problem_t<HerderState>
{
public:
	DecisionModel();
	virtual ~DecisionModel();

	void reset( Herder & agent, int daysUntilWetSeason, int horizon );

	// Interface inherited from Problem::problem_t<T>
	virtual Problem::action_t number_actions( const HerderState &s ) const;
	virtual const HerderState & init() const;
	virtual bool terminal( const HerderState & state ) const;
	virtual bool dead_end( const HerderState & state ) const;
	virtual bool applicable( const HerderState & state, Problem::action_t action ) const;
	virtual float cost( const HerderState &state, Problem::action_t action ) const;
	virtual void next( const HerderState & state, Problem::action_t action, OutcomeVector & outcomes ) const;
	
	virtual	void print( std::ostream& os ) const {}
protected:
	void makeActionsForState( HerderState& tates ) const;
	void applyFrameEffects( const HerderState& state,  HerderState& stateP ) const;

	Herder * _agent;
	HerderState * _initial;
	int _horizon;
	unsigned int _daysUntilWetSeason;

};

} // namespace GujaratCellphones

#endif // __DecisionModel_hxx__

