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
//	std::cout << "creating ForageAction with sector: " << _forageArea << " resources: " << _forageArea->getBiomassAmount() << std::endl;

//*?
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
	ForageAction * newAction = 0;
	
	//* According to the strategy of sharing sector structures
	// between MDP states, no MDPAction owns its ForageArea, so it
	// can not do any delete nor creation.
	/*
	if( !_ownsForageAreaPointer )
	{
		newAction = new ForageAction(_HRForageArea, _LRForageArea, false);
	}
	else
	{
		newAction = new ForageAction( new Sector( *_HRForageArea ), new Sector( *_LRForageArea ),true);
	}
	*/
	
	assert(_LRForageArea->cells().size() >0);
	
	newAction = new ForageAction(_HRForageArea, _LRForageArea, false);
	
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

void	ForageAction::selectBestWalk( const GujaratAgent& agent
				,const Engine::Point2D<int>& n
				,const Engine::Raster& r
				,int maxDistAgentWalk
				,std::vector< Engine::Point2D<int>* > & walk )
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
	
	for(unsigned int i=0; i < walk.size(); i++)
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
void	ForageAction::selectBestNearestHRCell( GujaratAgent& agent
					,const Engine::Point2D<int>& n
					,const Engine::Raster& r
					,int& bestScore
					,Engine::Point2D<int>& best ) const
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


void ForageAction::selectBestNearestHRCellInLRCell_ScanFrame( const GujaratWorld * gw
					, GujaratAgent&  agent
					, const Engine::Point2D<int>& LRn
					, const Engine::Point2D<int>& HRNearest
					, Engine::Raster& HRRes
					, int & bestScoreHR
					, Engine::Point2D<int>& bestHR )
{
	int FRAMESIZE = 40;
	Engine::Point2D<int> curr;
	Engine::Point2D<int> corner;
	int lowResolution = ((GujaratConfig)gw->getConfig()).getLowResolution();	
	gw->LowRes2HighResCellCorner(LRn, corner);
	
	//std::cout << "CORNER:" << LRn << " --> " << curr << std::endl;
	
	bestScoreHR = -1;
	long bestDistanceHR = gw->getOverlapBoundaries()._origin.distanceSQ(gw->getOverlapBoundaries()._origin + gw->getOverlapBoundaries()._size);
	
	std::vector < Engine::Point2D<int> > candidates(10);
	int numCandidates = 0;
	
	// Explore the intersection between the frame of neighbours and the cells 
	// contained in the LRCell
	int iInit  = (corner._x>HRNearest._x-FRAMESIZE)?corner._x:HRNearest._x-FRAMESIZE;
	int iBound = (corner._x+lowResolution-1<HRNearest._x+FRAMESIZE)?corner._x+lowResolution-1:HRNearest._x+FRAMESIZE;
	int jInit  = (corner._y>HRNearest._y-FRAMESIZE)?corner._y:HRNearest._y-FRAMESIZE;
	int jBound = (corner._y+lowResolution-1<HRNearest._y+FRAMESIZE)?corner._y+lowResolution-1:HRNearest._y+FRAMESIZE;
	
	for(int i = iInit; i <= iBound; i++)
	{
		curr._x = i;
		for(int j = jInit; j <= jBound; j++)
		{	
			curr._y = j;
			
			//std::cout << "CURRENT " << curr << " LOWRES=" << lowResolution << std::endl;
			
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
		//std::cout << " good? " << *sectorCells[k] << std::endl;
		
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
	//TODO cost n, instead: best=candidates[uniformRandom(0,candidates.size()-1)]
	//std::random_shuffle(candidates.begin(), candidates.end());
	int numCand = candidates.size();
	int idxCand = rand()%numCand;
	best = *candidates[idxCand];
//*?
//	best = *candidates[Engine::GeneralState::statistics().getUniformDistValue(0,candidates.size()-1)];

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
	
	//if(!wasInsideLR)
	//{
		// update "wasInsideLR". Once you enter the LR cell you
		// do not come out. So once "wasInsideLR" becomes true it
		// will be true always. Do not check re-update when it is true.
	wasInsideLR = 	HRBegin._x >= (LREndPoint._x*lowResolution) &&
			HRBegin._x < ((LREndPoint._x+1)*lowResolution) &&
			HRBegin._y >= (LREndPoint._y*lowResolution) &&
			HRBegin._y < ((LREndPoint._y+1)*lowResolution);
	//}
	
	if(wasInsideLR)
	{
				
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
	
	/*
	std::cout << "*******BEGIN: " << HRBegin <<" * " << home << std::endl;
	std::cout << "*******BEGIN2: " << (HRBegin - home) << std::endl;*/

	
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
		/*
			std::cout << "*******SECTOR: " << curr <<" * " << home << std::endl;
			std::cout << "*******SECTOR2: " << (curr - home) << std::endl;
		*/
			//std::cout << "CURRENT " << curr << " LOWRES=" << lowResolution << std::endl;

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


/*
Engine::Point2D<int> toAbs(int x, int y, int hx, int hy, int homerange) {

  Engine::Point2D<int> r;
  
  
  
  
 // r.x
  
  
  
}*/

void ForageAction::selectBestNearestHRCellInSector_ScanFrame(const GujaratWorld * gw
				, GujaratAgent&  agent					, const Engine::Point2D<int>& HRBegin			, Engine::Raster& HRRes					, int & bestScoreHR					, Engine::Point2D<int>& bestHR )
{
	const int FRAMESIZE = 40;
	const int HOMERANGE = ((HunterGatherer&)agent).getHomeRange();
	Engine::Point2D<int> curr;
	Engine::Point2D<int> home = agent.getPosition();
	Engine::Point2D<int> cornerLeftUp;
	Engine::Point2D<int> cornerDownRight;


	cornerLeftUp._x = HRBegin._x - FRAMESIZE;
	cornerLeftUp._y = HRBegin._y - FRAMESIZE; 

	cornerDownRight._x = HRBegin._x + FRAMESIZE;
	cornerDownRight._y = HRBegin._y + FRAMESIZE; 
	
	if(cornerLeftUp._x < home._x - HOMERANGE)	
		cornerLeftUp._x = (home._x - HOMERANGE);
	if(cornerLeftUp._y < home._y - HOMERANGE)	
		cornerLeftUp._y = (home._y - HOMERANGE);
	if( cornerDownRight._x > home._x + HOMERANGE )
		cornerDownRight._x = (home._x + HOMERANGE);
	if(cornerDownRight._y > home._y + HOMERANGE)	
		cornerDownRight._y = (home._y + HOMERANGE);
	
	if(cornerLeftUp._x < gw->getBoundaries()._origin._x)
	{
		cornerLeftUp._x = gw->getBoundaries()._origin._x;
	}
	if(cornerLeftUp._y < gw->getBoundaries()._origin._y)
	{
		cornerLeftUp._y = gw->getBoundaries()._origin._y;
	}

	if(cornerDownRight._x > gw->getBoundaries()._origin._x + gw->getBoundaries()._size._x -1 )
	{
		cornerDownRight._x = gw->getBoundaries()._origin._x + gw->getBoundaries()._size._x-1;
	}

	if(cornerDownRight._y > gw->getBoundaries()._origin._y + gw->getBoundaries()._size._y -1)
	{
		cornerDownRight._y = gw->getBoundaries()._origin._y + gw->getBoundaries()._size._y-1;
	}

	//SectorsMask & HRSectorsMask = GujaratState::getHRSectorsMask();

	/*
	std::cout << "*******BEGIN: " << HRBegin <<" * " << home << std::endl;
	std::cout << "*******BEGIN2: " << (HRBegin - home) << std::endl;*/

	int currentSector=-1;
	try{
	currentSector = GujaratState::sectorsMask(HRBegin._x-home._x+HOMERANGE,HRBegin._y-home._y+HOMERANGE, GujaratState::getHRSectorsMask());
	}catch(const std::exception & e)
			{
				Engine::Point2D<int> p = HRBegin - home;
				Engine::Point2D<int> q = p;
				q._x += HOMERANGE+1;
				q._y += HOMERANGE+1;

				std::cout << "EXCEPT:" << p << "|" << q << std::endl;
			}
	//std::cout << "CORNER:" << LRn << " --> " << curr << std::endl;

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

		/*
			std::cout << "*******SECTOR: " << curr <<" * " << home << std::endl;
			std::cout << "*******SECTOR2: " << (curr - home) << std::endl;
		*/

			try{
				if (currentSector != GujaratState::sectorsMask(
					curr._x - home._x+HOMERANGE
					,curr._y - home._y+HOMERANGE
					,GujaratState::getHRSectorsMask()))
				{
					continue;
				}
			}catch(const std::exception & e)
			{
				Engine::Point2D<int> q;

				q = curr - HRBegin;
				q = q + HOMERANGE;

				std::cout << "***EXCEPTION***" << curr << "|" << q << "|" << std::endl; 
				std::cout << " x= " << (curr._x - home._x+HOMERANGE) << "; y= " << curr._y - home._y+HOMERANGE << std::endl;
				std::cout << "Home Range: " << HOMERANGE << std::endl;
				std::cout << "Home cell: x= " << home._x << "; y= " << home._y << std::endl;
				exit(0);
			}

			//std::cout << "CURRENT " << curr << " LOWRES=" << lowResolution << std::endl;

			int currRes = HRRes.getValue(curr - gw->getOverlapBoundaries()._origin);

			if(currRes == bestScoreHR)
			{
				long d = curr.distanceSQ(HRBegin);
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
	
	//std::cout << "CORNER:" << LRn << " --> " << curr << std::endl;
	
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
			
			//std::cout << "CURRENT " << curr << " LOWRES=" << lowResolution << std::endl;
			
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





void	ForageAction::doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::Raster& r, int & collected ) 
{
	
//std::cout << "there are " << _LRForageArea->cells().size() << " cells" << std::endl;	
	
	
#ifdef CLASSICWALK
	doClassicalWalk( agent, n0, maxDist, r, collected );
#endif
	
/*	
#ifdef LRCLASSICWALK
	doLRClassicWalk( agent, n0, maxDist, r, collected );
#endif
*/

#ifdef LRVICINITYWALK
	doLRVicinityWalk( agent, n0, maxDist, r, collected );
#endif
	
#ifdef VICINITYWALK
	doVicinityWalk( agent, n0, maxDist, r, collected );
#endif
	
#ifdef TRENDVICINITYWALK
	doTrendVicinityWalk( agent, n0, maxDist, r, collected );
#endif
	
}

void	ForageAction::doTrendVicinityWalkForRewardEstimation( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& source_r, int & collected ) 
{
	Engine::IncrementalRaster r(source_r);
    
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
	
	
	//*? TODO eLRResources??? this is omniscience? should be _hgMind->resourceRaster
	
	selectBestNearestLRCell( agent, LRn, gw, agent.getWorld()->getDynamicRaster(eLRResources)
	, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
	, bestScore, LRBest );
	//std::cout << "2222222" << std::endl;
	// find endpoint
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
	
		
		//std::cout << "walked dist: " << walkedDist << " dist home: " << distHome << " max dist: " << maxDist << " biomass collected: " << collected << " calories: " << agent.convertBiomassToCalories(collected) << std::endl;
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
		//std::cout << "444444444" << std::endl;
		//bestScore = r.getValue(best - agent.getWorld()->getOverlapBoundaries()._origin); 

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

		//w++;
	}
	
	//std::cout << "res " << collected << " loops " << loops << " wd " << walkedDist << " dH " << distHome << " mD " << maxDist << " foraging dist: " << agent.getTimeSpentForagingTile() << " fm " << agent.getPopulationSize() << std::endl;
	//std::cout << "LOOP " << collected << "," << loops << "," << walkedDist << "," << distHome << "," << maxDist << std::endl;
	
	// update l'LRraster??? and LRsectors???
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


void	ForageAction::doTrendVicinityWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int & collected ) 
{
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
	
	
	//*? TODO eLRResources??? this is omniscience? should be _hgMind->resourceRaster
	
	selectBestNearestLRCell( agent, LRn, gw, agent.getWorld()->getDynamicRaster(eLRResources)
	, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
	, bestScore, LRBest );
	//std::cout << "2222222" << std::endl;
	// find endpoint
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
	
		
		//std::cout << "walked dist: " << walkedDist << " dist home: " << distHome << " max dist: " << maxDist << " biomass collected: " << collected << " calories: " << agent.convertBiomassToCalories(collected) << std::endl;
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
		//std::cout << "444444444" << std::endl;
		//bestScore = r.getValue(best - agent.getWorld()->getOverlapBoundaries()._origin); 

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

		//w++;
	}
	
	//std::cout << "res " << collected << " loops " << loops << " wd " << walkedDist << " dH " << distHome << " mD " << maxDist << " foraging dist: " << agent.getTimeSpentForagingTile() << " fm " << agent.getPopulationSize() << std::endl;
	//std::cout << "LOOP " << collected << "," << loops << "," << walkedDist << "," << distHome << "," << maxDist << std::endl;
	
	// update l'LRraster??? and LRsectors???
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


void	ForageAction::doVicinityWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int & collected ) 
{
	double walkedDist 	= 0.0;
	Engine::Point2D<int> n 	= n0;
	double distHome 	= 0.0;
	Engine::Point2D<int> best;
	int bestScore 		= 0;
	
	// needed by selectBestNearestHRCellInSector_ScanFrame
	selectBestNearestHRCell( agent, n0, r, bestScore, best );
	n = best;	
	
	do 
	{	
	
		selectBestNearestHRCellInSector_ScanFrame( 
			(GujaratWorld*)(agent.getWorld())
			,agent
			, n
			, r
			, bestScore
			, best );		

		//bestScore = r.getValue(best - agent.getWorld()->getOverlapBoundaries()._origin); 

		// 2. update walk distance
		walkedDist += agent.getTimeSpentForagingTile();
		walkedDist += best.distance(n);
		n = best;
		distHome = n0.distance(n);
		int amtCollected = agent.computeEffectiveBiomassForaged( bestScore );
		//int prevActivity = agent.getWorld()->getValue(eForageActivity, n );
		//agent.getWorld()->setValue(eForageActivity, n, prevActivity + 1 );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getOverlapBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getOverlapBoundaries()._origin, prevValue - amtCollected );

		//w++;
	} while ( ( walkedDist + distHome ) < maxDist );
	//std::cout << "LOOP " << collected << "," << loops << "," << walkedDist << "," << distHome << "," << maxDist << std::endl;
// update l'LRraster??? and LRsectors???
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


void	ForageAction::doLRVicinityWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int & collected ) 
{
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId();
	
	GujaratWorld * gw = (GujaratWorld *)agent.getWorld();
	
	Engine::Raster & LRRes = gw->getDynamicRaster(eLRResources);
	
	double walkedDist         = 0.0;
	Engine::Point2D<int> LRHome;
        gw->worldCell2LowResCell( agent.getPosition(), LRHome);
	Engine::Point2D<int> LRn  = LRHome;
	Engine::Point2D<int> LRBest = LRHome;
	double distHome           = 0.0;
	int bestScore;
	Engine::Point2D<int> HRNearest;
	Engine::Point2D<int> HRHome(agent.getPosition());
	
	selectBestNearestLRCell(agent, LRHome, gw, agent.getWorld()->getDynamicRaster(eLRResources)
	, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
	, bestScore, LRBest );
	
	do{
		
		// 2. update walk distance
		LRn               = LRBest;
		int cellResources = ((GujaratWorld*)agent.getWorld())->getValueLR(LRRes,LRn);
		walkedDist       = walkedDist + LRBest.distance(LRn);// distance 0, free for LR
		
		int numInterDune = ((GujaratWorld*)agent.getWorld())->getValueLR(LRCounterSoilINTERDUNE,LRn);
		int nonVisited   = numInterDune;
		//int prevActivity = ((GujaratWorld*)agent.getWorld())->getValueLR(eLRForageActivity, n );
		int amtCollected = 0;
		
		// Exploit LR cell
		gw->LowRes2HighResCellCorner(LRn,HRNearest);
		distHome = HRHome.distance(HRNearest);
		while ( nonVisited > 0 && (( walkedDist + distHome ) < maxDist) )
		{
			Engine::Point2D<int> bestHR;
			int bestScoreHR = 0;			

			//**************************************************************
			
			selectBestNearestHRCellInLRCell_ScanFrame( gw, agent, LRn, HRNearest, r, bestScoreHR, bestHR );
			
			//best = walk[w];
			//bestScore = r.getValue(best - agent.getWorld()->getOverlapBoundaries()._origin); 
			
			//*************************************************************
			// 2. update walk distance
			walkedDist += agent.getTimeSpentForagingTile();
			walkedDist += bestHR.distance(HRNearest);
			HRNearest = bestHR;
			distHome = HRHome.distance(HRNearest);	
			amtCollected = agent.computeEffectiveBiomassForaged( bestScoreHR );
			//std::cout << "collected: " << amtCollected << " from: " << bestScore << " walkeddist: " << walkedDist << " distHome: " << distHome << " maxdist: " << maxDist << std::endl;
			//int prevActivity = agent.getWorld()->getValue(eForageActivity, n );
			//agent.getWorld()->setValue(eForageActivity, n, prevActivity + 1 );
			collected += amtCollected;
			
			// 4. update cell resources & amount collected
			int prevValue = r.getValue(HRNearest - gw->getOverlapBoundaries()._origin); 
			r.setValue( HRNearest - gw->getOverlapBoundaries()._origin, prevValue - amtCollected );
			
			nonVisited--;
			
		}
		
		cellResources = cellResources - amtCollected;
		collected += amtCollected;
		//((GujaratWorld*)agent.getWorld())->setValueLR(eLRForageActivity, n, prevActivity + numInterDune - nonVisited);
		// 4. update cell resources & amount collected
		((GujaratWorld*)agent.getWorld())->setValueLR(LRRes,LRn,cellResources);
		bestScore = 0;
		selectBestNearestLRCell( 	agent
		, LRn
		, ((GujaratWorld*)agent.getWorld())
		, LRRes
		, (int)(agent.getAvailableTime() / agent.getForageTimeCost())
		, bestScore, LRBest );
		
	}while ( ( walkedDist + distHome ) < maxDist && (bestScore > 0));
	
	//TODO
	// update l'LRraster??? done a few lines below with setValueLR
	// update LRSectors??? (utility attribute) 	
}


void	ForageAction::doClassicalWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int & collected ) 
{
	double walkedDist = 0.0;
	Engine::Point2D<int> n = n0;
	double distHome = 0.0;
	std::stringstream logName;
	logName << agent.getWorld()->getId() << "_" << agent.getId();	
	
	int loops=0;
	while ( ( walkedDist + distHome ) < maxDist )
	{		
		loops++;
		
		Engine::Point2D<int> best;
		int bestScore = 0;
	
		selectBestNearestHRCell( agent, n, r, bestScore, best );		
		
		// 2. update walk distance
		walkedDist += agent.getTimeSpentForagingTile();
		walkedDist += best.distance(n);
		n = best;
		distHome = n0.distance(n);	
		int amtCollected = agent.computeEffectiveBiomassForaged( bestScore );
		//int prevActivity = agent.getWorld()->getValue(eForageActivity, n );
		//agent.getWorld()->setValue(eForageActivity, n, prevActivity + 1 );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getOverlapBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getOverlapBoundaries()._origin, prevValue - amtCollected );
		
		//w++;
	}
	
	//std::cout << "res " << collected << " loops " << loops << " wd " << walkedDist << " dH " << distHome << " mD " << maxDist << std::endl;
	//std::cout << "LOOP " << collected << "," << loops << "," << walkedDist << "," << distHome << "," << maxDist << std::endl;
	
	// update l'LRraster??? and LRsectors???
	// One action per timestep -> Next time I need LRraster and LRsectors they will be updated by
	// nextstep method in world -> do not update LRraster, LRsectors
}


void	ForageAction::doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, double maxDist, Engine::Raster& r, int& collected ) const
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
			amtCollected = amtCollected + std::min(cellResources,
					agent.computeEffectiveBiomassForagedNoStochastic( cellResources/numInterDune ));
					//*? agent.computeEffectiveBiomassForaged(...)
		
			walkedDist += agent.getTimeSpentForagingTile();
			
			nonVisited--;
		}
		
		cellResources = cellResources - amtCollected;
		collected += amtCollected;
		//((GujaratWorld*)agent.getWorld())->setValueLR(eLRForageActivity, n, prevActivity + numInterDune - nonVisited);
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
		
	}while ( ( walkedDist + distHome ) < maxDist && (bestScore > 0));
	
	//TODO
	// update l'LRraster??? done a few lines below with setValueLR
	// update LRSectors??? (utility attribute) 
	
}


 void ForageAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	
	assert(_LRForageArea->cells().size() >0);
	
	double  maxDist= agent.computeMaxForagingDistance(_useFullPopulation);
	
	Engine::Point2D<int> LRlocation;
	((GujaratWorld*)agent.getWorld())->worldCell2LowResCell( s.getLocation(), LRlocation );
	Engine::Point2D<int> nearest = LRlocation;
	//= _LRForageArea->getNearestTo( LRlocation );
	//((GujaratWorld*)agent.getWorld())->worldCell2LowResCell( s.getPosition(), nearest );
	
	//Engine::Point2D<int> nearest = _LRForageArea->getNearestTo( s.getLocation() );

	assert(_LRForageArea->cells().size() > 0);
	
	
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



#ifdef REDUCC
bool ForageAction::equal(MDPAction * a,const GujaratAgent & gua) 
{ 	
//return dynamic_cast<ForageAction*>(a);
	
		return dynamic_cast<ForageAction*>(a) 
			&& 
				_ownsForageAreaPointer == ((ForageAction*)a)->_ownsForageAreaPointer
//			&& 
//				gua.reductionResourcesToCategory( gua.convertBiomassToCalories(_biomassCollected) )
//				== gua.reductionResourcesToCategory(gua.convertBiomassToCalories(((ForageAction*)a)->_biomassCollected))
			&& 
				gua.reductionResourcesToCategory(_caloriesCollected) == gua.reductionResourcesToCategory(((ForageAction*)a)->_caloriesCollected)
			&& 
				_useFullPopulation == ((ForageAction*)a)->_useFullPopulation			
			&& 
				_LRForageArea->_direction == ((ForageAction*)a)->_LRForageArea->_direction;		

		}	
#endif

#ifndef REDUCC
bool ForageAction::equal(MDPAction * a,const GujaratAgent & gua) 
{ 	
		return dynamic_cast<ForageAction*>(a) 
			&& 
				_ownsForageAreaPointer == ((ForageAction*)a)->_ownsForageAreaPointer
			&& 
				_biomassCollected == ((ForageAction*)a)->_biomassCollected
			&& 
				_caloriesCollected == ((ForageAction*)a)->_caloriesCollected
			&& 
				_useFullPopulation == ((ForageAction*)a)->_useFullPopulation			
			&& 
				_LRForageArea->_direction == ((ForageAction*)a)->_LRForageArea->_direction;		

		}	
#endif









}
