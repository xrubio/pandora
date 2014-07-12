#include <ForageAction.hxx>
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

ForageAction::ForageAction( Sector* loc, bool ownsPointer )
	: _forageArea( loc ), _ownsForageAreaPointer( ownsPointer ), _biomassCollected(0), _caloriesCollected(0), _useFullPopulation(true)
{
//	std::cout << "creating ForageAction with sector: " << _forageArea << " resources: " << _forageArea->getBiomassAmount() << std::endl;
}

ForageAction::~ForageAction()
{
	if ( _ownsForageAreaPointer )
	{
		delete _forageArea;
	}
}

MDPAction*	ForageAction::copy() const
{
	ForageAction * newAction = 0;
	if( !_ownsForageAreaPointer )
	{
		newAction = new ForageAction(_forageArea, false);
	}
	else
	{
		newAction = new ForageAction( new Sector( *_forageArea ), true);
	}
	newAction->setFullPopulation(_useFullPopulation);
	return newAction;
}

std::string ForageAction::describe() const
{
	std::stringstream logForage;
	logForage << "forage(" << _forageArea->getBiomassAmount() << ")," << getBiomassCollected() << "," << getCaloriesCollected();
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
	Engine::Point2D<int> nearest = _forageArea->getNearestTo( agent.getPosition() );

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
}

void	ForageAction::selectBestNearestCell( 	const Engine::Point2D<int>& n,
						const Engine::DynamicRaster& r,
						int& bestScore,
						Engine::Point2D<int>& best ) const
{
	bestScore = 0;
	std::vector< Engine::Point2D<int> > candidates;	
	double minDist = std::numeric_limits<double>::max();

	const std::vector< Engine::Point2D<int> >& sectorCells = _forageArea->cells();

	for ( unsigned k = 0; k < _forageArea->numCells(); k++ )
	{
		int score =  r.getValue( sectorCells[k] - _forageArea->getWorld().getBoundaries()._origin );
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

void	ForageAction::doWalk( GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::DynamicRaster& r, int& collected ) 
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
		//std::cout << "collected: " << amtCollected << " from: " << bestScore << " walkeddist: " << walkedDist << " distHome: " << distHome << " maxdist: " << maxDist << std::endl;
		int prevActivity = agent.getWorld()->getValue(eForageActivity, n );
		agent.getWorld()->setValue(eForageActivity, n, prevActivity + 1 );
		collected += amtCollected;

		// 4. update cell resources & amount collected
		int prevValue = r.getValue(n - agent.getWorld()->getBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getBoundaries()._origin, prevValue - amtCollected );
	}
}


void	ForageAction::doWalk( const GujaratAgent& agent, const Engine::Point2D<int>& n0, 
				double maxDist, Engine::DynamicRaster& r, int& collected ) const
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
		int prevValue = r.getValue(n - agent.getWorld()->getBoundaries()._origin); 
		r.setValue( n - agent.getWorld()->getBoundaries()._origin, prevValue - amtCollected );
	}
}

 void ForageAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const
{
	double  maxDist= agent.computeMaxForagingDistance(_useFullPopulation);
		
	Engine::Point2D<int> nearest = _forageArea->getNearestTo( s.getLocation() );

	int collected = 0;

	doWalk( agent, nearest, maxDist, sp.getResourcesRaster(), collected );

	sp.addResources( agent.convertBiomassToCalories(collected));
	//std::cout << "executeMDP - time index: " << s.getTimeIndex() << " ForageAction with full pop: " << _useFullPopulation << " from area with max biomass: " << _forageArea->getBiomassAmount() << " loc->resources: " << s.getLocation() << " -> " << s.getOnHandResources() << " to: " << sp.getLocation() << " -> " << sp.getOnHandResources() << " days starving: " << s.getDaysStarving() << std::endl;
}

void ForageAction::setFullPopulation( bool useFullPopulation )
{
	_useFullPopulation = useFullPopulation;
}

}
