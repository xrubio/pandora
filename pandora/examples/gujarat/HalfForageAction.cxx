#include <HalfForageAction.hxx>
#include <Agent.hxx>
#include <GujaratAgent.hxx>
#include <HunterGatherer.hxx>
#include <GujaratWorld.hxx>
#include <HunterGathererMDPState.hxx>
#include <cassert>

#include <Logger.hxx>
#include <GeneralState.hxx>

namespace Gujarat
{

HalfForageAction::HalfForageAction( Sector* loc, bool ownsPointer )
	: _forageArea( loc ), _ownsForageAreaPointer( ownsPointer ), _biomassCollected(0), _caloriesCollected(0)//, _useFullPopulation(true)
{
}

HalfForageAction::~HalfForageAction()
{
	if ( _ownsForageAreaPointer )
		delete _forageArea;
}

MDPAction*	HalfForageAction::copy() const
{
	if ( !_ownsForageAreaPointer )
		return new HalfForageAction( _forageArea, false );
	
	return new HalfForageAction( new Sector( *_forageArea ), true );
}

std::string HalfForageAction::describe() const
{
	std::stringstream logForage;
	logForage << "half forage(" << _forageArea->biomassClass() << ")," << getBiomassCollected() << "," << getCaloriesCollected();
	return logForage.str();
}


void HalfForageAction::execute( Engine::Agent & a )
{
	std::stringstream logName;
	logName << a.getWorld()->getId() << "_" << a.getId() << "_actions";
	log_DEBUG(logName.str(), " executing Half Forage action"); 

	HunterGatherer& agent = (HunterGatherer&) a;

	std::stringstream logName2;
	logName2 << "actions_" << agent.getWorld()->getId();
	log_INFO(logName2.str(), "timestep: " << agent.getWorld()->getCurrentTimeStep() << " agent: " << agent << " executes HalfForage");

	// 1. collect nr adults
	double  maxDistAgentWalk = agent.computeMaxForagingDistance(false); //_useFullPopulation )
		
	// 2. select nearest cell
	Engine::Point2D<int> nearest = _forageArea->getNearestTo( agent.getPosition() );

	// 3. execute walk
	_biomassCollected = 0;
	doWalk( (GujaratAgent&)a, nearest, maxDistAgentWalk, agent.getWorld()->getDynamicRaster("resources"), _biomassCollected );	

	_caloriesCollected = agent.convertBiomassToCalories( _biomassCollected );
	agent.updateResources( _caloriesCollected );
	std::cout << agent << " executing forage calories: " << _caloriesCollected << " with half pop and needs: " << agent.computeConsumedResources(1) << std::endl;
}

void	HalfForageAction::selectBestNearestCell( 	const Engine::Point2D<int>& n,
						const Engine::Raster& r,
						int& bestScore,
						Engine::Point2D<int>& best ) const
{
	bestScore = 0;
	std::vector< Engine::Point2D<int> > candidates;	
	double minDist = std::numeric_limits<double>::max();

	const std::vector< Engine::Point2D<int> >& sectorCells = _forageArea->cells();

	for ( unsigned k = 0; k < _forageArea->numCells(); k++ )
	{
		int score =  r.getValue( sectorCells[k] - _forageArea->getWorld().getOverlapBoundaries()._origin );
		double dist = sectorCells[k].distance(n);
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
	std::random_shuffle(candidates.begin(), candidates.end());
	best = candidates[0];
}

void	HalfForageAction::doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::Raster& r, int& collected ) 
{
	double walkedDist = 0.0;
	Engine::Point2D<int> n = n0;
	double distHome = 0.0;

	while ( ( walkedDist + distHome ) < maxDist )
	{
		//std::cout << "walked dist: " << walkedDist << " dist home: " << distHome << " max dist: " << maxDist << " biomass collected: " << collected << " calories: " << agent.convertBiomassToCalories(collected) << std::endl;
		Engine::Point2D<int> best;
		int bestScore = 0;
		selectBestNearestCell( n, r, bestScore, best );
		// 2. update walk distance
		walkedDist += agent.getTimeSpentForagingTile();
		walkedDist += best.distance(n);
		n = best;
		distHome = n0.distance(n);	
		int amtCollected = agent.computeEffectiveBiomassForaged( bestScore );
		//int prevActivity = agent.getWorld()->getValue( "forageActivity", n );
		//agent.getWorld()->setValue( "forageActivity", n, prevActivity + 1 );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getOverlapBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getOverlapBoundaries()._origin, prevValue - amtCollected );
	}
}


void	HalfForageAction::doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::Raster& r, int& collected ) const
{
	double walkedDist = 0.0;
	Engine::Point2D<int> n = n0;
	double distHome = 0.0;

	while ( ( walkedDist + distHome ) < maxDist )
	{
		Engine::Point2D<int> best;
		int bestScore = 0;
		selectBestNearestCell( n, r, bestScore, best );
		// 2. update walk distance
		walkedDist += agent.getTimeSpentForagingTile();
		walkedDist += best.distance(n);
		n = best;
		distHome = n0.distance(n);	
		int amtCollected = agent.computeEffectiveBiomassForaged( bestScore );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getOverlapBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getOverlapBoundaries()._origin, prevValue - amtCollected );
	}
}

 void HalfForageAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	double  maxDist= agent.computeMaxForagingDistance();
		
	Engine::Point2D<int> nearest = _forageArea->getNearestTo( s.getLocation() );

	int collected = 0;

	doWalk( agent, nearest, maxDist, sp.getResourcesRaster(), collected );

	sp.addResources( agent.convertBiomassToCalories(collected));
}

/*
void HalfForageAction::setFullPopulation( bool useFullPopulation )
{
	_useFullPopulation = useFullPopulation;
}
*/

}
