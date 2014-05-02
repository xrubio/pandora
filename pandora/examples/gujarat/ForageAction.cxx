#include <stdlib.h>  

#include <ForageAction.hxx>
#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <HunterGatherer.hxx>
#include <GujaratConfig.hxx>
#include <HunterGathererMDPState.hxx>
#include <GujaratState.hxx>
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
		assert(_LRForageArea->cells().size() >0);
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
	/**
	 Holds:
	 Copy Method is not a deep copy.
	 Forage are passed as pointers.	 
	 */
	assert(_LRForageArea->cells().size() >0);
	
	/**
	// Require:
	// In case of applying shared sector structures between MDP states, 
	// no MDPAction owns its ForageArea, so it can not do any delete 
	// nor creation. The "ownsPointer" must be false.
	*/
	ForageAction* newAction = new ForageAction(_HRForageArea, _LRForageArea, false);
	newAction->setFullPopulation(_useFullPopulation);
	return newAction;
}


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


	// 1. collect nr adults
	double  maxDistAgentWalk = agent.computeMaxForagingDistance( _useFullPopulation );
		
	// 2. select nearest cell
	Engine::Point2D<int> nearest = *_HRForageArea->getNearestTo( agent.getPosition() );

	// 3. execute walk
	_biomassCollected = doWalk( (GujaratAgent&)a, nearest, maxDistAgentWalk, agent.getWorld()->getDynamicRaster(eResources));
	_caloriesCollected = agent.convertBiomassToCalories( _biomassCollected );
	agent.updateResources( _caloriesCollected );
	
	// A "Half forage" associated to Move Home is not using full pop
	if(_useFullPopulation)
	{
		agent.incrementForageActionsExecuted();
	}
	
	// TODO ask advice about the best place to put that call
	// 1.- end of ForageAction::execute
	// 2.- in a loop before executing actions at HunterGatherer::executeActions
	//agent.updateKnowledgeFromFirstHandVisit(_LRForageArea);
}





void	ForageAction::selectBestNearestLRCell( const GujaratAgent& agent
					,const Engine::Point2D<int>& n
					,const GujaratWorld *gw
					,Engine::Raster& resourceRaster
					,int explorableCells
					,int& bestScore
					,Engine::Point2D<int>& best ) const
{
		
	bestScore = 0;
	std::vector< Engine::Point2D<int>* > candidates;
	double minDist = std::numeric_limits<double>::max();

	const std::vector< Engine::Point2D<int>* >& sectorCells = _LRForageArea->cells();
			
			
	assert(_LRForageArea->cells().size() > 0);

	for ( unsigned k = 0; k < _LRForageArea->numCells(); k++ )
	{	
		
		/* Unfold of score function :
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

		int score = LRcellOutcomeHeuristic(*sectorCells[k], gw, explorableCells,resourceRaster);

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
	int numCand = candidates.size();
	int idxCand = rand()%numCand;
	best = *candidates[idxCand];

	candidates.clear();
}

void ForageAction::selectBestNearestHRCellInTrend_ScanFrame(
				const GujaratWorld * gw
				, GujaratAgent&  agent
				, const Engine::Point2D<int>& HRBegin
				, Engine::Point2D<int>& HREndPoint
				, const Engine::Point2D<int>& LREndPoint
				, Engine::Raster& HRRes
				, bool & wasInsideLR
				, int & bestScoreHR
				, Engine::Point2D<int>& bestHR )
{
	int FRAMESIZE = 40;
	int HOMERANGE = ((HunterGatherer&)agent).getHomeRange();
	Engine::Point2D<int> curr;
	Engine::Point2D<int> home = agent.getPosition();
	Engine::Point2D<int> cornerLeftUp;
	Engine::Point2D<int> cornerDownRight;
	int lowResolution = ((GujaratConfig)gw->getConfig()).getLowResolution();
	
	wasInsideLR = 	HRBegin._x >= (LREndPoint._x*lowResolution) &&
			HRBegin._x < ((LREndPoint._x+1)*lowResolution) &&
			HRBegin._y >= (LREndPoint._y*lowResolution) &&
			HRBegin._y < ((LREndPoint._y+1)*lowResolution);
	
	if(wasInsideLR)
	{
		/*? review		
		/*cornerLeftUp._x = (LREndPoint._x*lowResolution)
		cornerLeftUp._y = (LREndPoint._y*lowResolution)
		cornerDownRight._x = cornerLeftUp._x + lowResolution;
		cornerDownRight._y = cornerLeftUp._y + lowResolution;
		*/
		
		cornerLeftUp._x = HRBegin._x - FRAMESIZE;
		cornerLeftUp._y = HRBegin._y - FRAMESIZE; 		
		if(cornerLeftUp._x < (LREndPoint._x*lowResolution))
		{
			cornerLeftUp._x = (LREndPoint._x*lowResolution);
		}

		if(cornerLeftUp._y < (LREndPoint._y*lowResolution))
		{
			cornerLeftUp._y = (LREndPoint._y*lowResolution);
		}

		cornerDownRight._x = HRBegin._x + FRAMESIZE;
		cornerDownRight._y = HRBegin._y + FRAMESIZE; 
		if(cornerDownRight._x >= ((LREndPoint._x+1)*lowResolution))
		{
			cornerDownRight._x = ((LREndPoint._x+1)*lowResolution) - 1;
		}

		if(cornerDownRight._y >= ((LREndPoint._y+1)*lowResolution))
		{
			cornerDownRight._y = ((LREndPoint._y+1)*lowResolution) - 1;
		}		
		// Inside LR cell always take the nearest to the last visited place.
		HREndPoint = HRBegin;	
	}
	else
	{
		cornerLeftUp._x = HRBegin._x - FRAMESIZE;
		cornerLeftUp._y = HRBegin._y - FRAMESIZE; 
		Engine::Rectangle<int> boundaries = gw->getOverlapBoundaries();	

	
		if(cornerLeftUp._x < boundaries._origin._x)
		{
			cornerLeftUp._x = boundaries._origin._x;
		}

		if(cornerLeftUp._y < boundaries._origin._y)
		{
			cornerLeftUp._y = boundaries._origin._y;
		}

		cornerDownRight._x = HRBegin._x + FRAMESIZE;
		cornerDownRight._y = HRBegin._y + FRAMESIZE; 
		if(cornerDownRight._x > boundaries._origin._x + boundaries._size._x -1 )
		{
			cornerDownRight._x = boundaries._origin._x + boundaries._size._x-1;
		}

		if(cornerDownRight._y > boundaries._origin._y + boundaries._size._y -1)
		{
			cornerDownRight._y = boundaries._origin._y + boundaries._size._y-1;
		}
		
		// Intersection versus homerange boundaries
		if(cornerLeftUp._x < home._x - HOMERANGE)
		{
			cornerLeftUp._x = home._x - HOMERANGE;
		}

		if(cornerLeftUp._y < home._y - HOMERANGE)
		{
			cornerLeftUp._y = home._y - HOMERANGE;
		}

		if(cornerDownRight._x > home._x + HOMERANGE)
		{
			cornerDownRight._x = home._x + HOMERANGE;
		}

		if(cornerDownRight._y > home._y + HOMERANGE)
		{
			cornerDownRight._y = home._y + HOMERANGE;
		}
		
	}
	
	
	bestScoreHR = -1;
	long bestDistanceHR = cornerLeftUp.distanceSQ(cornerDownRight);

	std::vector < Engine::Point2D<int> > candidates(10);
	int numCandidates = 0;

	// Explore the intersection between the frame of neighbours and the cells 
	// contained in the LRCell
	int iInit  = cornerLeftUp._x;
	int iBound = cornerDownRight._x;
	int jInit  = cornerLeftUp._y;
	int jBound = cornerDownRight._y;

	for(int i = iInit; i <= iBound; i++)
	{
		curr._x = i;
		for(int j = jInit; j <= jBound; j++)
		{
			curr._y = j;

			int currRes = HRRes.getValue(curr - gw->getOverlapBoundaries()._origin);

			if(currRes == bestScoreHR)
			{
				long d = curr.distanceSQ(HREndPoint);
				if(d < bestDistanceHR)
				{
					//candidates.clear();
					numCandidates = 0;
					bestScoreHR = currRes;
					bestDistanceHR = d;

					candidates[numCandidates] = curr;
					numCandidates++;
				}
				else if(d == bestDistanceHR)
				{
					candidates[numCandidates] = curr;
					numCandidates++;
					bestDistanceHR = d;
				}
			}
			else if (currRes > bestScoreHR)
			{
				//candidates.clear();
				numCandidates = 0;
				bestScoreHR = currRes;

				candidates[numCandidates] = curr;
				numCandidates++;
			}
		}
	}


	try{
	bestHR = candidates[Engine::GeneralState::statistics().getUniformDistValue(0,numCandidates-1)];
	}catch(const std::exception & e)
			{
				std::cout << "EXCP: candidates" << std::endl;
			}
	candidates.clear();
}

void ForageAction::selectBestNearestHRCellInLRCell_ScanAllLRCell( 
					const GujaratWorld * gw
					, GujaratAgent&  agent
					, const Engine::Point2D<int>& LRn
					, const Engine::Point2D<int>& HRNearest
					, Engine::Raster& HRRes
					, int & bestScoreHR
					, Engine::Point2D<int>& bestHR )
{
	Engine::Point2D<int> curr;
	Engine::Point2D<int> corner;
	
	gw->LowRes2HighResCellCorner(LRn, corner);
	
	bestScoreHR = -1;
	long bestDistanceHR = gw->getOverlapBoundaries()._origin.distanceSQ(gw->getOverlapBoundaries()._origin + gw->getOverlapBoundaries()._size);
	
	
	std::vector < Engine::Point2D<int> > candidates(10);
	int numCandidates = 0;
	
	int lowResolution = ((GujaratConfig)gw->getConfig()).getLowResolution();
	for(int i = 0; i < lowResolution; i++)
	{
		curr._x = corner._x + i;
		for(int j = 0; j < lowResolution; j++)
		{	
			curr._y = corner._y + j;
			
			int currRes = HRRes.getValue(curr - gw->getOverlapBoundaries()._origin);
			
			if(currRes == bestScoreHR)
			{
				long d = curr.distanceSQ(HRNearest);
				if(d < bestDistanceHR)
				{
					//candidates.clear();
					numCandidates = 0;
					bestScoreHR = currRes;
					bestDistanceHR = d;
					
					candidates[numCandidates] = curr;
					numCandidates++;
				}
				else if(d == bestDistanceHR)
				{					
					candidates[numCandidates] = curr;
					numCandidates++;
					bestDistanceHR = d;
				}
			}
			else if (currRes > bestScoreHR)
			{
				//candidates.clear();
				numCandidates = 0;
				bestScoreHR = currRes;
				
				candidates[numCandidates] = curr;
				numCandidates++;
			}
		}
	}
	
	bestHR = candidates[Engine::GeneralState::statistics().getUniformDistValue(0,numCandidates-1)];
	
	candidates.clear();
}


int ForageAction::doWalkForRewardEstimation( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, const Engine::Raster& r) {
	Engine::IncrementalRaster rasterCopy(r);
	return doWalk(agent, n0, maxDist, rasterCopy);
}


int ForageAction::doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r) 
{
	int collected = 0;
	
	double walkedDist 	= 0.0;
	Engine::Point2D<int> n 	= n0;
	double distHome 	= 0.0;
	Engine::Point2D<int> best;
	int bestScore 		= 0;
		
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId();
	
	GujaratWorld * gw = (GujaratWorld *)agent.getWorld();
	
	Engine::Point2D<int> HRHome(agent.getPosition());
	Engine::Point2D<int> LRHome;
	gw->worldCell2LowResCell( agent.getPosition(), LRHome);
	Engine::Point2D<int> LRn    = LRHome;
	Engine::Point2D<int> LRBest;	
	Engine::Point2D<int> HRNearest;
	
	
	//* why not eLRResources??? it is about the omniscience feature
	// HGMind must be taken into account
	// change agent.getWorld()->getDynamicRaster(eLRResources)
	// by
	// agent.getLRResourcesRaster()
	selectBestNearestLRCell( agent, LRn, gw, agent.getLRResourcesRaster()
	, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
	, bestScore, LRBest );
	
	
	// Find endpoint
	int lowResolution = ((GujaratConfig)gw->getConfig()).getLowResolution();
	Engine::Point2D<int> HREndPoint= LRBest*lowResolution;
	if (HREndPoint._x >= HRHome._x )
	{
		HREndPoint._x = HREndPoint._x + lowResolution -1;
	}
	if (HREndPoint._y >= HRHome._y )
	{
		HREndPoint._y = HREndPoint._y + lowResolution -1;
	}	
	bool wasInsideLR;
	
	while ( ( walkedDist + distHome ) < maxDist )
	{			
		selectBestNearestHRCellInTrend_ScanFrame( 
			(GujaratWorld*)(agent.getWorld())
			,agent
			, n
			, HREndPoint
			, LRBest
			, r
			, wasInsideLR
			, bestScore
			, best );		

		// 2. update walk distance
		walkedDist += agent.getTimeSpentForagingTile();
		walkedDist += best.distance(n);
		n = best;
		distHome = n0.distance(n);
		int amtCollected = agent.computeEffectiveBiomassForaged( bestScore );
		int prevActivity = agent.getWorld()->getValue(eForageActivity, n );
		agent.getWorld()->setValue(eForageActivity, n, prevActivity + 1 );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getOverlapBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getOverlapBoundaries()._origin, prevValue - amtCollected );
	}
	return collected;
	
	// Is it needed to update utility in LRraster and LRsectors?
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


int ForageAction::doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r) const
{
	// To be called from UCT part
	int collected = 0;
	
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

		// This loop could be simplified to a single formula based on sum of normals simplification.
		while ( nonVisited > 0 && (( walkedDist + distHome ) < maxDist) )
		{
			amtCollected = amtCollected + std::min(cellResources,
					agent.computeEffectiveBiomassForagedNoStochastic( cellResources/numInterDune ));	
		
			walkedDist += agent.getTimeSpentForagingTile();
			
			nonVisited--;
		}
		
		cellResources = cellResources - amtCollected;
		collected += amtCollected;
		// 4. update cell resources & amount collected
		((GujaratWorld*)agent.getWorld())->setValueLR(r,n,cellResources);
		bestScore = 0;
		selectBestNearestLRCell( 	agent
					, n
					, ((GujaratWorld*)agent.getWorld())
					, r
					, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
					, bestScore
					, best );
		
	} while ( ( walkedDist + distHome ) < maxDist && (bestScore > 0));
	return collected;
	
	// Is it needed to update utility in LRraster and LRsectors?
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


 void ForageAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{	
	assert(_LRForageArea->cells().size() >0);
	
	double  maxDist= agent.computeMaxForagingDistance(_useFullPopulation);
	
	Engine::Point2D<int> LRlocation;
	((GujaratWorld*)agent.getWorld())->worldCell2LowResCell( s.getLocation(), LRlocation );
	Engine::Point2D<int> nearest = LRlocation;
	
	assert(_LRForageArea->cells().size() > 0);
	
	int collected = doWalk( agent, nearest, maxDist, sp.getResourcesRaster());
	sp.addResources( agent.convertBiomassToCalories(collected));
}

void ForageAction::setFullPopulation( bool useFullPopulation )
{
	_useFullPopulation = useFullPopulation;
}

}
