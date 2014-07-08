#ifndef __HunterGathererMDPState_hxx__
#define __HunterGathererMDPState_hxx__

#include <Point2D.hxx>
#include <IncrementalRaster.hxx>
#include <HashTable.hxx>
#include <MDPAction.hxx>
#include <engine/problem.h>

namespace Gujarat
{

class HunterGathererMDPState
{
public:
	// Constructors, I don't want this to be ever invoked
	explicit HunterGathererMDPState();

	// The real one
	HunterGathererMDPState(Engine::Point2D<int> loc, int initialOnHand, const Engine::DynamicRaster& resourcesRaster, int maxResources, int divider);
	HunterGathererMDPState( const HunterGathererMDPState& s );
	const HunterGathererMDPState&	operator=(const HunterGathererMDPState& s );
	
	~HunterGathererMDPState();

	void		initializeSuccessor( HunterGathererMDPState& s ) const;

	unsigned	hash() const;
	bool		operator==( const HunterGathererMDPState& s ) const;
	bool		operator!=( const HunterGathererMDPState& s ) const;
	bool		operator<( const HunterGathererMDPState& s ) const;

	void		print( std::ostream& os ) const;

	void					increaseTimeIndex() { _timeIndex++; }
	unsigned				getTimeIndex() const { return _timeIndex; }
	int					getOnHandResources() const { return _onHandResources; }
	void addResources( int amt )
	{
		_onHandResources += amt;
		/*
		if(_onHandResources > _resourcesDivider*_maxResources)
		{
			_onHandResources = _resourcesDivider*_maxResources;
		}
		*/
		/*
		_onHandResources = _onHandResources + (amt / _resourcesDivider); 
		if ( _onHandResources > _maxResources )
			_onHandResources = _maxResources;
			*/
	}

	void consume() 
	{ 
		if( _onHandResources < _resourcesDivider )
		{
			_daysStarving += 1000.0f*(1.0f-((float)_onHandResources/(float)_resourcesDivider));
			//std::cout << "consume of this: " << this << " with days starving: " << _daysStarving << std::endl;
			//_onHandResources -= _resourcesDivider;
			//_daysStarving=0;
		}
		//_onHandResources = 0;
		/*
		else
		{
			_onHandResources = 0;
			_daysStarving++;
		}
		*/
	}
	
	float getDaysStarving() const
	{
		//std::cout << "getDaysStarving for: " << this << " days starving: " << _daysStarving << " fraction: " << (float)_daysStarving/1000.0f << std::endl;
		return (float)_daysStarving/1000.0f;
	}

	//void					spoilage( float v ) { _onHandResources = (float)_onHandResources * v; }
	void					setLocation( Engine::Point2D<int> newLoc ) { _mapLocation = newLoc; }
	const Engine::Point2D<int>&		getLocation() const { return _mapLocation; }
	Engine::IncrementalRaster&		getResourcesRaster() { return _resources; }
	const Engine::IncrementalRaster&	getResourcesRaster() const { return _resources; }

	void		addAction( MDPAction* a );
	MDPAction*		availableActions( Problem::action_t actIndex ) { return _availableActions.at(actIndex); }
	const MDPAction*	availableActions( Problem::action_t actIndex ) const { return _availableActions.at(actIndex); }

	unsigned	numAvailableActions() const { return _availableActions.size(); }

	void	computeHash();
private:
	

private:
	unsigned			_timeIndex;
	Engine::Point2D<int>		_mapLocation;
	int				_onHandResources;
	Engine::IncrementalRaster	_resources;
	Engine::HashKey			_hashKey;
	std::vector<MDPAction*>		_availableActions;
	int				_maxResources;
	int				_resourcesDivider;
	int _daysStarving;
	bool				_isCopy;
};

inline std::ostream& operator<<( std::ostream& os, const HunterGathererMDPState& s )
{
	s.print(os);
	return os;	
}

}

#endif // HunterGathererMDPState.hxx
