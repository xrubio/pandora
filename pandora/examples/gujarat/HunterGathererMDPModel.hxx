#ifndef __HUNTERGATHERER_MDP_MODEL__
#define __HUNTERGATHERER_MDP_MODEL__

#include <HunterGathererMDPState.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <engine/problem.h>
#include <vector>

namespace Gujarat
{

class HunterGatherer;

typedef std::vector< std::pair< HunterGathererMDPState, float > >	OutcomeVector; 

class HunterGathererMDPModel : public Problem::problem_t< HunterGathererMDPState >
{
public:
	HunterGathererMDPModel(const HunterGathererMDPConfig& cfg);
	virtual ~HunterGathererMDPModel();

	void	reset( GujaratAgent & agent );

	// Interface inherited from Problem::problem_t<T>
	virtual Problem::action_t number_actions( const HunterGathererMDPState &s ) const;
	virtual const HunterGathererMDPState & init() const;
	virtual bool terminal( const HunterGathererMDPState &s ) const;
	virtual bool dead_end( const HunterGathererMDPState &s ) const { return false; }
	virtual bool applicable( const HunterGathererMDPState &s, Problem::action_t a ) const;
	virtual float cost( const HunterGathererMDPState &s, Problem::action_t a ) const;
	virtual void next( const HunterGathererMDPState &s, Problem::action_t a, OutcomeVector& outcomes ) const;
	virtual	void print( std::ostream& os ) const {}
	
protected:
	void	applyFrameEffects( const HunterGathererMDPState& s,  HunterGathererMDPState& sp ) const;

	HunterGatherer*			_agent;
	HunterGathererMDPState*		_initial;
	const HunterGathererMDPConfig& _config;
};

}

#endif
