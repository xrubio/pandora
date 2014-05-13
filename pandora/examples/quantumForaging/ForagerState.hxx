
#ifndef __ForagerState_hxx__
#define __ForagerState_hxx__

#include <DynamicRaster.hxx>
#include <IncrementalRaster.hxx>
#include <Point2D.hxx>
#include <HashTable.hxx>
#include <engine/problem.h>
#include "BaseAction.hxx"

namespace QuantumExperiment
{

class ForagerState
{
public:
	ForagerState(const Engine::Point2D<int> & position,  int neededResources, const Engine::DynamicRaster& knowledgeMap, const Engine::DynamicRaster& resourcesMap, int timeStep);
	ForagerState( const ForagerState & state);
	virtual ~ForagerState();

	unsigned int hash() const;

	const ForagerState & operator=(const ForagerState & state );
	bool operator==( const ForagerState & state ) const;
	
	void increaseTimeStep();
	int getTimeStep() const;
	int getNumAvailableActions() const;

	const Engine::Point2D<int> & getPosition() const;
	void setPosition( const Engine::Point2D<int> & position );

	void addAction( BaseAction * action );
	const BaseAction & getAvailableAction(Problem::action_t index) const;

	void consume();
	void computeHash();

	void setForagedResources( int foragedResources );
	int getForagedResources() const;
	int getNeededResources() const;
	Engine::IncrementalRaster & getKnowledgeMap();
	const Engine::IncrementalRaster & getKnowledgeMap() const;
	Engine::IncrementalRaster & getResourcesMap();
	const Engine::IncrementalRaster & getResourcesMap() const;
	
	// this method randomize the order of actions. It is useful in cases where two actions are equal, in ordere to avoid artifacts where the first one is always chosen
	void randomizeActions();
	void clearActions();
private:
	// time step where the state is created
	int _timeStep;
	Engine::Point2D<int> _position;
	float _starvation;
	int _foragedResources;

	Engine::IncrementalRaster _knowledgeMap;
	Engine::IncrementalRaster _resourcesMap;

	Engine::HashKey	* _hashKey;
	std::vector<BaseAction *> _availableActions;
	int _neededResources;
	bool _isCopy;
public:
	friend std::ostream & operator<<( std::ostream & stream, ForagerState & state );
	friend std::ostream & operator<<( std::ostream & stream, const ForagerState & state );

};

} // namespace QuantumExperiment

#endif // __ForagerState_hxx__

