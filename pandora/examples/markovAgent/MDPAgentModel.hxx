
#ifndef __MDPAgentModel_hxx__
#define __MDPAgentModel_hxx__

#include <engine/problem.h>

namespace Examples
{
class MDPAgent;
class MDPAgentState;

typedef std::vector<std::pair<MDPAgentState,float> > OutcomeVector; 

class MDPAgentModel : public Problem::problem_t<MDPAgentState>
{
public:
	MDPAgentModel();
	virtual ~MDPAgentModel();

	void setup( const unsigned int & horizon );
	void reset( MDPAgent & agent );

	// Interface inherited from Problem::problem_t<T>
	virtual Problem::action_t number_actions( const MDPAgentState &s ) const;
	virtual const MDPAgentState & init() const;
	virtual bool terminal( const MDPAgentState & state ) const;
	virtual bool dead_end( const MDPAgentState & state ) const;
	virtual bool applicable( const MDPAgentState & state, Problem::action_t action ) const;
	virtual float cost( const MDPAgentState &state, Problem::action_t action ) const;
	virtual void next( const MDPAgentState & state, Problem::action_t action, OutcomeVector & outcomes ) const;
	
	virtual	void print( std::ostream& os ) const {}
protected:
	void makeActionsForState( MDPAgentState& tates ) const;
	void applyFrameEffects( const MDPAgentState& state,  MDPAgentState& stateP ) const;

	MDPAgent * _agent;
	MDPAgentState * _initial;
	unsigned int _horizon;
};

} // namespace Examples

#endif // __MDPAgentModel_hxx__

