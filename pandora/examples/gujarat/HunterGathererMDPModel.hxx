#ifndef __HUNTERGATHERER_MDP_MODEL__
#define __HUNTERGATHERER_MDP_MODEL__

#include <HunterGathererMDPState.hxx>
#include <HunterGathererMDPConfig.hxx>
#include <engine/problem.h>
#include <vector>
#include <map>

namespace Gujarat
{

class ForageAction;
class MoveHomeAction;
class DoNothingAction;
class HunterGatherer;
class HunterGathererMDPConfig;

typedef std::vector< std::pair< HunterGathererMDPState, float > >	OutcomeVector; 

class HunterGathererMDPModel : public Problem::problem_t< HunterGathererMDPState >
{
public:

	HunterGathererMDPModel();
	virtual ~HunterGathererMDPModel();

	void 	setup( const HunterGathererMDPConfig& cfg );
	void	reset( GujaratAgent & agent );

	HunterGatherer& 	agentRef()
	{
		return *(_simAgent);
	}

	const HunterGatherer& 	agentRef() const
	{
		return *(_simAgent);
	}

	unsigned getHorizon() const 
	{
		return _horizon;
	}

	void	setHorizon( unsigned H )
	{
		_horizon = H;
	}

	unsigned getWidth() const 
	{
		return _width;
	}

	void	setWidth( unsigned W )
	{
		_width = W;
	}	

	void clean()
	{
		//_simAgent = 0;
		delete _initial;
		_initial=NULL;
	}
	
	// Interface inherited from Problem::problem_t<T>
	virtual Problem::action_t number_actions( const HunterGathererMDPState &s ) const;
	virtual const HunterGathererMDPState & init() const;
	virtual bool terminal( const HunterGathererMDPState &s ) const;
	virtual bool dead_end( const HunterGathererMDPState &s ) const 
		{ return false; }
	virtual bool applicable( const HunterGathererMDPState &s, Problem::action_t a ) const;
	virtual float cost( const HunterGathererMDPState &s, Problem::action_t a ) const;
	virtual void next( const HunterGathererMDPState &s
			, Problem::action_t a
			, OutcomeVector& outcomes ) const;
	
	virtual void next( const HunterGathererMDPState &s
			, Problem::action_t a
			, OutcomeVector& outcomes
			 , int foo ) const;
			
			
	virtual	void print( std::ostream& os ) const {}
	
protected:
	
	//void makeActionsForState( HunterGatherer& parent, const Engine::Point2D<int> &loc, std::vector<MDPAction *>&  actionList) const;
	
	void makeActionsForState( 
				const HunterGathererMDPState& parent
				, const Engine::Point2D<int> &loc
				, std::vector< Sector* > * HRActionSectors
				, std::vector< Sector* > * LRActionSectors
				, std::vector< Engine::Point2D<int> > * HRCellPool
				, std::vector< Engine::Point2D<int> > * LRCellPool
				, std::vector<MDPAction *>&  actionList) const;

	void makeActionsForState(
			      const Engine::IncrementalRaster & resourcesRaster			      
			      , const Engine::Point2D<int> &position
			      , std::vector< Sector* >* HRActionSectors
			      , std::vector< Sector* >* LRActionSectors
			      , std::vector< Engine::Point2D<int> >* HRCellPool
			      , std::vector< Engine::Point2D<int> >* LRCellPool
			      , std::vector<MDPAction *>& actionList) const;
	
	void	applyFrameEffects( const HunterGathererMDPState& s,  HunterGathererMDPState& sp ) const;
private:

	HunterGatherer*			_simAgent;
	HunterGathererMDPState*		_initial;
	HunterGathererMDPConfig		_config;
	unsigned			_horizon;
	unsigned			_width;
};

}

#endif
