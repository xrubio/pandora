
#ifndef __MDPAgentState_hxx__
#define __MDPAgentState_hxx__

#include <Raster.hxx>
#include <IncrementalRaster.hxx>
#include <Point2D.hxx>
#include <HashTable.hxx>
#include <engine/problem.h>

namespace Examples
{

class MoveAction;

class MDPAgentState
{
public:
	explicit MDPAgentState();
	MDPAgentState(const Engine::Point2D<int> & position, int resources, const Engine::Raster& resourcesRaster, unsigned int horizon, int resourcesToEat);
	MDPAgentState( const MDPAgentState & state );
	virtual ~MDPAgentState();

	void initializeSuccessor( MDPAgentState & state ) const;
	unsigned int hash() const;

	const MDPAgentState & operator=(const MDPAgentState & state );
	bool operator==( const MDPAgentState & state ) const;
	bool operator!=( const MDPAgentState& state ) const;
	bool operator<( const MDPAgentState & state ) const;
	
	void increaseTimeStep();
	int getTimeStep() const;
	int getNumAvailableActions() const;
	const Engine::Point2D<int> & getPosition() const;
	void setPosition( const Engine::Point2D<int> & position );

	void addAction( MoveAction * action );
	const MoveAction & getAvailableAction(Problem::action_t index) const;
	void eat();
	void computeHash();

	int getResources() const;
	void setResources( int value );
	const Engine::IncrementalRaster & getRasterResources() const;
	Engine::IncrementalRaster & getRasterResources();
	// this method randomize the order of actions. It is useful in cases where two actions are equal, in ordere to avoid artifacts where the first one is always chosen
	void randomizeActions();

private:
	int _timeStep;
	Engine::Point2D<int> _position;
	int _resources;
	Engine::IncrementalRaster _rasterResources;
	Engine::HashKey	_hashKey;
	std::vector<MoveAction*> _availableActions;
	int _horizon;
	int _resourcesToEat;
	bool _isCopy;
public:
	friend std::ostream & operator<<( std::ostream & stream, MDPAgentState & state );
	friend std::ostream & operator<<( std::ostream & stream, const MDPAgentState & state );

};

}

#endif // __MDPAgentState_hxx__

