#ifndef __ForageAction_hxx__
#define __ForageAction_hxx__


#include <MDPAction.hxx>
#include <Sector.hxx>
#include <Point2D.hxx>
#include <GujaratWorld.hxx>

#include <algorithm>
#include <functional>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{
	
class GujaratAgent;


class ForageAction : public MDPAction
{
	
	Sector* _HRForageArea;
	Sector* _LRForageArea;
	
	bool _ownsForageAreaPointer;
	int	_biomassCollected;
	int	_caloriesCollected;
	bool _useFullPopulation;

		
	void selectBestNearestCell( GujaratAgent& agent, 
								const Engine::Point2D<int>& n, 
								const Engine::Raster& r, 
								int& bestScore, 
								Engine::Point2D<int>& best ) const; 
	
	void selectBestNearestCell( const GujaratAgent& agent, 
								const Engine::Point2D<int>& n,
								const GujaratWorld *gw,
								Engine::Raster& resourceRaster,
								int explorableCells,
								int& bestScore,
								Engine::Point2D<int>& best ) const;
							 
	void selectBestWalk( const GujaratAgent& agent, 
						const Engine::Point2D<int>& n,
						const Engine::Raster& r,
						int maxDistAgentWalk,
						std::vector< Engine::Point2D<int>* > & walk );
	
	void doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int& collected ) const;
	void doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int& collected );
	
	
	
public:
	ForageAction( Sector* HRLoc, Sector* LRLoc, bool ownsPointer = false );
	virtual ~ForageAction();

	// if fullPopulation = false, only half the individuals will contribute to the action (the rest is moving home)
	void execute( Engine::Agent& agent);
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;

	int	getTimeNeeded() const { return 1; }
	virtual MDPAction* copy() const;
	virtual std::string describe() const;
	
	int	getBiomassCollected() const { return _biomassCollected; }
	int	getCaloriesCollected() const { return _caloriesCollected; }
	
	void setFullPopulation( bool useFullPopulation );
	const Sector & getHRSector() const {return *_HRForageArea;}
	const Sector & getLRSector() const {return *_LRForageArea;}

	int LRcellOutcomeHeuristic( Engine::Point2D<int> kCell2D
								, const Gujarat::GujaratWorld * gw
								, int explorableCells
								, const Engine::Raster& resRaster) const
	{
		long   n                          = gw->getValueLR(LRCounterSoilINTERDUNE,kCell2D);
		int    res                        = gw->getValueLR(resRaster,kCell2D);
		float  meanRes                    = ((float)res) / ((float)n);
		int    expectedExploredCells      = std::min((int)n,explorableCells);
		float  expectedRetrievedResources = meanRes * expectedExploredCells;		
		int    score                      = expectedRetrievedResources;
	/*std::cout << "score heuristic:" << n 
					<< "," << res
					<< "," << meanRes
					<< "," << expectedExploredCells
					<< "," << expectedRetrievedResources
					<< "," << score << std::endl;*/

	return score;
	}

	Sector * getVisitedSector() { return _LRForageArea; }
	
	//******************
	//** Add-ons for doWalk efficiency
	
	struct distance_from_home : public std::binary_function<Engine::Point2D<int>*,Engine::Point2D<int>*,bool>
	{
		Engine::Point2D<int> _homeCell;
		
		inline bool operator() (const Engine::Point2D<int> * p1, const Engine::Point2D<int> * p2) 
		{
			return ( _homeCell.distanceSQ(*p1) < _homeCell.distanceSQ(*p2) );
			//return ( r.getValue( struct1 ) < r.getValue( struct2 ));
		}
	};
	

	struct less_than : public std::binary_function<Engine::Point2D<int>*,Engine::Point2D<int>*,bool>
	{		
		const Engine::Raster * _workingRaster;
		GujaratWorld * _workingWorld;
	
		//add inline keyword
		inline bool operator() (const Engine::Point2D<int> * p1, const Engine::Point2D<int> * p2) 
		{
			return ( _workingWorld->getValueGW(*_workingRaster, *p1 ) 
			< _workingWorld->getValueGW(*_workingRaster, *p2 ));
			/*return ( _workingWorld->World::getValue(eResources, p1 ) 
			< _workingWorld->World::getValue(eResources, p2 ));*/
		}
	};
	
	//Gujarat::GujaratWorld::getValue(Engine::Raster&, const Engine::Point2D<int>&)
	
	//int Engine::World::getValue(const int&, const Engine::Point2D<int>&) const
	
	//**********************
	//** ownership of sector structures int the MDP tree
	
	void getOwnershipMDPSectorKnowledge(std::vector<bool> & v) const
	{
		v[0] = false;// HRSectors
		v[1] = false;// HRPool
		v[2] = true;// LRSectors
		v[3] = false;// LRPools
	}
	
};


}

#endif // ForageAction.hxx

