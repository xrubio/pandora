#include <ForageAction.hxx>
#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <HunterGatherer.hxx>
//#include <GujaratWorld.hxx>
#include <HunterGathererMDPState.hxx>
#include <cassert>

#include <Logger.hxx>
#include <GeneralState.hxx>

#include<iterator>
#include<algorithm>
#include<fstream>
#include<iostream>
#include<vector>


namespace Gujarat
{
	ForageAction::ForageAction( Sector* HRLoc, Sector* LRLoc, bool ownsPointer )
	: _HRForageArea( HRLoc ), _LRForageArea( LRLoc ), _ownsForageAreaPointer( ownsPointer ), _biomassCollected(0), _caloriesCollected(0), _useFullPopulation(true)
{
//	std::cout << "creating ForageAction with sector: " << _forageArea << " resources: " << _forageArea->getBiomassAmount() << std::endl;
}

ForageAction::~ForageAction()
{
	if ( _ownsForageAreaPointer )
	{
		delete _HRForageArea;
		delete _LRForageArea;
	}
}

MDPAction*	ForageAction::copy() const
{
	ForageAction * newAction = 0;
	if( !_ownsForageAreaPointer )
	{
		newAction = new ForageAction(_HRForageArea, _LRForageArea, false);
	}
	else
	{
		newAction = new ForageAction( new Sector( *_HRForageArea ), new Sector( *_LRForageArea ),true);
	}
	newAction->setFullPopulation(_useFullPopulation);
	return newAction;
}

/*
MDPAction*	ForageAction::shallowCopy() const
{
	ForageAction * newAction = 0;
	if( !_ownsForageAreaPointer )
	{
		newAction = new ForageAction(_HRForageArea, _LRForageArea, false);
	}
	else
	{
		newAction = new ForageAction( new Sector( *_HRForageArea ), new Sector( *_LRForageArea ),true);
	}
	newAction->setFullPopulation(_useFullPopulation);
	return newAction;
}
*/

std::string ForageAction::describe() const
{
	std::stringstream logForage;
	logForage << "forage(" << _HRForageArea->getBiomassAmount() << ","<< _HRForageArea->getBiomassAmount() << ")," << getBiomassCollected() << "," << getCaloriesCollected();
	return logForage.str();
}


void ForageAction::execute( Engine::Agent & a )
{
	std::stringstream logName;
	logName << a.getWorld()->getId() << "_" << a.getId() << "_actions";
	log_DEBUG(logName.str(), " executing Forage action"); 

	HunterGatherer& agent = (HunterGatherer&) a;

	/*
	std::stringstream logName2;
	logName2 << "actions_" << agent.getWorld()->getId();
	log_INFO(logName2.str(), "timestep: " << agent.getWorld()->getCurrentTimeStep() << " agent: " << agent << " executes Forage with pop: " << _useFullPopulation);
	*/

	// 1. collect nr adults
	double  maxDistAgentWalk = agent.computeMaxForagingDistance( _useFullPopulation );
		
	// 2. select nearest cell
	Engine::Point2D<int> nearest = *_HRForageArea->getNearestTo( agent.getPosition() );

	// 3. execute walk
	_biomassCollected = 0;
	doWalk( (GujaratAgent&)a, nearest, maxDistAgentWalk, agent.getWorld()->getDynamicRaster(eResources), _biomassCollected );	

	//std::cout << "biomass collected: " << _biomassCollected << " to calories: " << agent.convertBiomassToCalories(_biomassCollected) << std::endl;
	_caloriesCollected = agent.convertBiomassToCalories( _biomassCollected );
	agent.updateResources( _caloriesCollected );
	//std::cout << agent << " executing forage in zone with max: " << _forageArea->getBiomassAmount() << " collected calories: " << _caloriesCollected << " with pop: " << _useFullPopulation << " and needs: " << agent.computeConsumedResources(1) << std::endl;
	// if not using full pop, is a movehome
	if(_useFullPopulation)
	{
		agent.incrementForageActionsExecuted();
	}
	/*
	std::cout << "MSG> FORAGE: "
		<< "t=" << a.getWorld()->getCurrentTimeStep() << ","
		<< "pos=" << a.getPosition() << ","
		<< "n0=" << nearest << ","
		<< "res=" << _biomassCollected
		<< std::endl;
	*/	
		/*
		std::cout << "MSG> SECTORDUMP:";
		const 	std::vector< Engine::Point2D<int> > & cells = _HRForageArea->cells();
		std::copy(cells.begin(),cells.end(),std::ostream_iterator<Engine::Point2D<int> > (std::cout," ")); 
		std::cout << std::endl;
		*/
	
	// TODO ask advice about the best place to put that call
	// 1.- end of ForageAction::execute
	// 2.- in a loop before executing actions at HunterGatherer::executeActions
	//agent.updateKnowledgeFromFirstHandVisit(_LRForageArea);
}

void	ForageAction::selectBestWalk( const GujaratAgent& agent, 
									  const Engine::Point2D<int>& n,
								   const Engine::Raster& r,
								   int maxDistAgentWalk,
								   std::vector< Engine::Point2D<int>* > & walk )
{
	double minDist = std::numeric_limits<double>::max();
	
	std::vector< Engine::Point2D<int>* >& sectorCells = _HRForageArea->cellsNoConst();
	
	// ordering the cells based on the values/resources
	
	struct less_than lt;
	lt._workingRaster = &r;
	lt._workingWorld  = (GujaratWorld*)agent.getWorld();
	std::sort(sectorCells.begin(), sectorCells.end(), lt);
	
	// getting the best maxDistAgentWalk cells -> resize?()
	walk.resize(maxDistAgentWalk);
	
	// ordering the cells based on the distances from Home
	//agent.getWorld()->sort(walk.begin(), walk.end(), distance_from_home());
	
	for(int i=0; i < walk.size(); i++)
	{
		walk[i] = sectorCells[i];
	}
	
	struct distance_from_home dfh;
	dfh._homeCell = n;
	std::sort(walk.begin(), walk.end(), dfh);
	
	/*
	 Maybe it is posible that walk is not needed, just sort first by resources,
	 afterwards, sort sector cells by distance in the components from 0 to maxDistAgentWalk 
	 */	
}



// ForageAction::selectBestNearestCell with const agent parameter applies LR operations
// ForageAction::selectBestNearestCell without const agent parameter applies HR operations
//TODO : one only ForageAction::selectBestNearestCell, add parameter forageArea
// the caller will put either LRForageArea, HRForageArea, as needed.
void	ForageAction::selectBestNearestCell( GujaratAgent& agent, 
											 const Engine::Point2D<int>& n,
											 const Engine::Raster& r,
											 int& bestScore,
											 Engine::Point2D<int>& best ) const
{
	bestScore = 0;
	std::vector< Engine::Point2D<int>* > candidates;	
	double minDist = std::numeric_limits<double>::max();

	const std::vector< Engine::Point2D<int>* >& sectorCells = _HRForageArea->cells();

	for ( unsigned k = 0; k < _HRForageArea->numCells(); k++ )
	{
		int score =  r.getValue( *sectorCells[k] - _HRForageArea->getWorld().getOverlapBoundaries()._origin );
		double dist = sectorCells[k]->distance(n);
		if ( score > bestScore )
		{
			bestScore = score;
			candidates.clear();
			candidates.push_back(sectorCells[k]);
			minDist = dist; 
		}
		else if(score == bestScore)
		{
			if(dist<minDist)
			{
				candidates.clear();
				candidates.push_back(sectorCells[k]);
				minDist = dist;
			}
			else if(minDist==dist)
			{
				candidates.push_back(sectorCells[k]);
			}
		}
	}
	
	//std::random_shuffle(candidates.begin(), candidates.end());
	//best = candidates[0];
	best = *candidates[Engine::GeneralState::statistics().getUniformDistValue(0,candidates.size()-1)];
	
	candidates.clear();
}

// ForageAction::selectBestNearestCell with const agent parameter applies LR operations
// ForageAction::selectBestNearestCell without const agent parameter applies HR operations
void	ForageAction::selectBestNearestCell( const GujaratAgent& agent, 
											 const Engine::Point2D<int>& n,
											 const GujaratWorld *gw,
										     Engine::Raster& resourceRaster,
										     int explorableCells,
										     int& bestScore,
										     Engine::Point2D<int>& best ) const
{
	bestScore = 0;
	std::vector< Engine::Point2D<int>* > candidates;	
	double minDist = std::numeric_limits<double>::max();
	
	const std::vector< Engine::Point2D<int>* >& sectorCells = _LRForageArea->cells();
	
	for ( unsigned k = 0; k < _LRForageArea->numCells(); k++ )
	{	
		/* Rationale for score function :
		 *			
		 *			// before calling selectBest...
		 *			t = free time
		 *			tc = time to explore 1 cell
		 *			explorableCells = t / tc
		 *			
		 *			// computed at LRcellOutcomeHeuristic
		 *			n = # resource cells in area
		 *			r = amount of resource in cell
		 *			xx = expected explored cells = min(n, explorableCells) 
		 *			m = mean resources per cell = r/n
		 *			xr = expected retrieved resources =  m*xx
		 *			score = xr
		 */
		
		int score = LRcellOutcomeHeuristic(*sectorCells[k], gw, explorableCells
		,resourceRaster);
		
		double dist = sectorCells[k]->distance(n);
		if ( score > bestScore )
		{
			bestScore = score;
			candidates.clear();
			candidates.push_back(sectorCells[k]);
			minDist = dist; 
		}
		else if(score == bestScore)
		{
			if(dist<minDist)
			{
				candidates.clear();
				candidates.push_back(sectorCells[k]);
				minDist = dist;
			}
			else if(minDist==dist)
			{
				candidates.push_back(sectorCells[k]);
			}
		}
	}
	//TODO cost n, instead: best=candidates[uniformRandom(0,candidates.size()-1)]
	//std::random_shuffle(candidates.begin(), candidates.end());
	best = *candidates[Engine::GeneralState::statistics().getUniformDistValue(0,candidates.size()-1)];

	candidates.clear();	
}


void	ForageAction::doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::Raster& r, int & collected ) 
{
	double walkedDist = 0.0;
	Engine::Point2D<int> n = n0;
	double distHome = 0.0;
	
	//std::vector< Engine::Point2D<int>* > walk;
	//selectBestWalk( agent,n0,r,maxDist,walk);
	//int w = 0;
	
	while ( ( walkedDist + distHome ) < maxDist )
	{		
		//*?
		/*
		std::cout << "MSG> WALK:";
		std::cout << " maxDist: " << maxDist;
		std::cout << " walked dist: " << walkedDist << " dist home: " << distHome << " max dist: " << maxDist << " biomass collected: " << collected << std::endl;
		*/
		//std::cout << "walked dist: " << walkedDist << " dist home: " << distHome << " max dist: " << maxDist << " biomass collected: " << collected << " calories: " << agent.convertBiomassToCalories(collected) << std::endl;
		
		Engine::Point2D<int> best;
		int bestScore = 0;
		selectBestNearestCell( agent, n, r, bestScore, best );
		
		//best = walk[w];
		//bestScore = r.getValue(best - agent.getWorld()->getOverlapBoundaries()._origin); 
		
		
		// 2. update walk distance
		walkedDist += agent.getTimeSpentForagingTile();
		walkedDist += best.distance(n);
		n = best;
		distHome = n0.distance(n);	
		int amtCollected = agent.computeEffectiveBiomassForaged( bestScore );
		//std::cout << "collected: " << amtCollected << " from: " << bestScore << " walkeddist: " << walkedDist << " distHome: " << distHome << " maxdist: " << maxDist << std::endl;
		//int prevActivity = agent.getWorld()->getValue(eForageActivity, n );
		//agent.getWorld()->setValue(eForageActivity, n, prevActivity + 1 );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getOverlapBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getOverlapBoundaries()._origin, prevValue - amtCollected );
		
		//w++;
	}
	/*std::cout << "MSG> WALK:";
	std::cout << " maxDist: " << maxDist;
	std::cout << " walked dist: " << walkedDist << " dist home: " << distHome << " max dist: " << maxDist << " biomass collected: " << collected << std::endl;
	*/
	// update l'LRraster??? and LRsectors???
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


void	ForageAction::doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::Raster& r, int& collected ) const
{
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId();
		
	double walkedDist         = 0.0;
	Engine::Point2D<int> n    = n0;
	Engine::Point2D<int> best = n0;
	double distHome           = 0.0;
	int bestScore             = LRcellOutcomeHeuristic(best
										, (GujaratWorld*)agent.getWorld()
										, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
										, r);
		
	do{
		
		// 2. update walk distance
		n                = best;
		int cellResources = ((GujaratWorld*)agent.getWorld())->getValueLR(r,n);
		walkedDist       = walkedDist + best.distance(n);
		distHome         = n0.distance(n);
		int numInterDune = ((GujaratWorld*)agent.getWorld())->getValueLR(LRCounterSoilINTERDUNE,n);
		int nonVisited   = numInterDune;
		int prevActivity = ((GujaratWorld*)agent.getWorld())->getValueLR(eLRForageActivity, n );
		int amtCollected = 0;

		// Exploit LR cell, SINGLE FORMULA version       
		// (bestScore/numInterDune)*(agent.getAvailableTime()/agent.getTimeSpentForagingTile());
/*		int visitable     = std::min(numInterDune, (int)(agent.getAvailableTime()/agent.getTimeSpentForagingTile()));
		int maxForage     = (bestScore * visitable) / numInterDune;
		amtCollected  = std::min(bestScore,(int)Engine::GeneralState::statistics().getNormalDistValue(0, maxForage));       
		walkedDist += agent.getTimeSpentForagingTile() * visitable;
*/		
		
		while ( nonVisited > 0 && (( walkedDist + distHome ) < maxDist) )
		{
			amtCollected = amtCollected + std::min(cellResources, agent.computeEffectiveBiomassForaged( cellResources/numInterDune ));
		
			/*std::cout << "MSG> RES:" 
				<< amtCollected << ","
				<< cellResources << ","
				<< numInterDune << ","
				<< std::endl;
			*/
			walkedDist += agent.getTimeSpentForagingTile();
			
			nonVisited--;
		}
		
		cellResources = cellResources - amtCollected;
		collected += amtCollected;
		//((GujaratWorld*)agent.getWorld())->setValueLR(eLRForageActivity, n, prevActivity + numInterDune - nonVisited);
		// 4. update cell resources & amount collected
		((GujaratWorld*)agent.getWorld())->setValueLR(r,n,cellResources);
		bestScore = 0;
		selectBestNearestCell( 	agent
								, n
								, ((GujaratWorld*)agent.getWorld())
								, r
								, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
								, bestScore, best );
		
	}while ( ( walkedDist + distHome ) < maxDist && (bestScore > 0));
	
	//TODO
	// update l'LRraster??? done a few lines below with setValueLR
	// update LRSectors??? (utility attribute) 
	
}


 void ForageAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	double  maxDist= agent.computeMaxForagingDistance(_useFullPopulation);
	
	Engine::Point2D<int> LRlocation;
	((GujaratWorld*)agent.getWorld())->worldCell2LowResCell( s.getLocation(), LRlocation );
	Engine::Point2D<int> nearest = LRlocation;
	//= _LRForageArea->getNearestTo( LRlocation );
	//((GujaratWorld*)agent.getWorld())->worldCell2LowResCell( s.getPosition(), nearest );
	
	//Engine::Point2D<int> nearest = _LRForageArea->getNearestTo( s.getLocation() );

	int collected = 0;

	doWalk( agent, nearest, maxDist, sp.getResourcesRaster(), collected );
		
	sp.addResources( agent.convertBiomassToCalories(collected));
	//std::cout << "executeMDP - time index: " << s.getTimeIndex() << " ForageAction with full pop: " << _useFullPopulation << " from area with max biomass: " << _forageArea->getBiomassAmount() << " loc->resources: " << s.getLocation() << " -> " << s.getOnHandResources() << " to: " << sp.getLocation() << " -> " << sp.getOnHandResources() << " days starving: " << s.getDaysStarving() << std::endl;
	/*
	std::cout << "MSG> MDPFORAGE: "
	<< "t=" << s.getTimeIndex() << ","
	<< "pos=" << s.getLocation() << ","
	<< "n0=" << nearest << ","
	<< "res=" << collected
	<< std::endl;
	*/
}

void ForageAction::setFullPopulation( bool useFullPopulation )
{
	_useFullPopulation = useFullPopulation;
}

}
