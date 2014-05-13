
#ifndef __HerderState_hxx__
#define __HerderState_hxx__

#include <DynamicRaster.hxx>
#include <IncrementalRaster.hxx>
#include <Point2D.hxx>
#include <HashTable.hxx>
#include <engine/problem.h>

namespace GujaratCellphones
{

class MoveAction;

class HerderState
{
public:
	HerderState(const Engine::Point2D<int> & position, const Engine::DynamicRaster& knowledgeMap, const Engine::DynamicRaster& resourcesMap, int resourcesToEat );
	HerderState( const HerderState & state);
	virtual ~HerderState();

	unsigned int hash() const;

	const HerderState & operator=(const HerderState & state );
	bool operator==( const HerderState & state ) const;
	
	int getNumAvailableActions() const;
	const Engine::Point2D<int> & getPosition() const;
	const Engine::Point2D<int> & getVillagePosition() const;
	void setPosition( const Engine::Point2D<int> & position );

	void addAction( MoveAction * action );
	const MoveAction & getAvailableAction(Problem::action_t index) const;
	void computeHash();

//	const Engine::IncrementalRaster & getRasterResources() const;
//	Engine::IncrementalRaster & getRasterResources();
	const Engine::IncrementalRaster & getKnowledgeMap() const;
	Engine::IncrementalRaster & getResourcesMap();
	const Engine::IncrementalRaster & getResourcesMap() const;
	// this method randomize the order of actions. It is useful in cases where two actions are equal, in ordere to avoid artifacts where the first one is always chosen
	void randomizeActions();
	void clearActions();
private:
	Engine::Point2D<int> _position;
	Engine::Point2D<int> _villagePosition;

	Engine::IncrementalRaster _resourcesMap;
	Engine::IncrementalRaster _knowledgeMap;

	Engine::HashKey	* _hashKey;
	std::vector<MoveAction*> _availableActions;
	int _resourcesToEat;
	bool _isCopy;
public:
	friend std::ostream & operator<<( std::ostream & stream, HerderState & state );
	friend std::ostream & operator<<( std::ostream & stream, const HerderState & state );

};

} // namespace GujaratCellphones

#endif // __HerderState_hxx__

