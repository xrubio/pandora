#ifndef __HalfForageAction_hxx__
#define __HalfForageAction_hxx__

#include <MDPAction.hxx>
#include <Sector.hxx>
#include <Point2D.hxx>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{

class GujaratAgent;
class GujaratWorld;

class HalfForageAction : public MDPAction
{
	Sector* _forageArea;
	bool _ownsForageAreaPointer;
	int	_biomassCollected;
	int	_caloriesCollected;
//	bool _useFullPopulation;

	void selectBestNearestCell( const Engine::Point2D<int>& n, const Engine::Raster& r, int& bestScore, Engine::Point2D<int>& best ) const; 

	void doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int& collected ) const;
	void doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int& collected );

public:
	HalfForageAction( Sector* loc, bool ownsPointer = false );
	virtual ~HalfForageAction();

	// if fullPopulation = false, only half the individuals will contribute to the action (the rest is moving home)
	void execute( Engine::Agent& agent);
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;

	int	getTimeNeeded() const { return 1; }
	virtual MDPAction* copy() const;
	virtual std::string describe() const;
	
	int	getBiomassCollected() const { return _biomassCollected; }
	int	getCaloriesCollected() const { return _caloriesCollected; }
	
//	void setFullPopulation( bool useFullPopulation );
};

}

#endif // HalfForageAction.hxx
