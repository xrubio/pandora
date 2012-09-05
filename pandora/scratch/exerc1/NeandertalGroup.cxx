
#include "NeandertalGroup.hxx"
#include "World.hxx"
#include "Exceptions.hxx"

#include <algorithm>
#include <cstring>

#include "Statistics.hxx"
#include "NEDWorld.hxx"

namespace NED
{

NeandertalGroup::NeandertalGroup( const std::string & id ) 
    : Agent(id)
    , _resources(0)
    , _population(0)
    , _homeRange(0)
    , _cannibalism(0)
    , _stepsUsingCannibalism(0)
    , _age(0)
    , _splitChance(0)
    , _children(0)
    , _minForkResources(0)
    , _splitWaste(0)
    , _maxAge(0)
    , _minFoodToEat(0)
    , _minFoodToMove(0)
{
}


NeandertalGroup::NeandertalGroup( const NeandertalGroupPackage & package ) : Agent(package._id)	
{  
	// params
	_minForkResources = package._minForkResources;
	_splitWaste       = package._splitWaste;
	_maxAge           = package._maxAge;
	_minFoodToEat     = package._minFoodToEat;
	_minFoodToMove    = package._minFoodToMove;
	
	// state  
	_position    = package._position;	
	_resources   = package._resources;
	_exists      = package._exists;
	_population  = package._population;
	_homeRange   = package._homeRange;
	_cannibalism = package._cannibalism;
	_stepsUsingCannibalism = package._stepsUsingCannibalism;
	_age         = package._age;
	_splitChance = package._splitChance;
	_children    = package._children;
	
	//TODO no faltaria _world???
	
}


NeandertalGroup::NeandertalGroup( const std::string & id 
				  , Engine::Point2D<int> position
				  , bool exists
				  , Engine::World* world
				  , int resources
				  , int population
				  , int homeRange
				  , int cannibalism
				  , int stepsUsingCannibalism
				  , int age
				  , int splitChance
				  , int children
				  , int minForkResources
				  , int splitWaste
				  , int maxAge
				  , int minFoodToEat
				  , int minFoodToMove) 
    : Agent(id)
    , _resources(resources)
    , _population(population)
    , _homeRange(homeRange)
    , _cannibalism(cannibalism)
    , _stepsUsingCannibalism(stepsUsingCannibalism)
    , _age(age)
    , _splitChance(splitChance)
    , _children(children)
    , _minForkResources(minForkResources)
    , _splitWaste(splitWaste)
    , _maxAge(maxAge)
    , _minFoodToEat(minFoodToEat)
    , _minFoodToMove(minFoodToMove)
{
_position = position;
_exists   = exists;
_world    = world;
}


NeandertalGroup::~NeandertalGroup()
{
}

void NeandertalGroup::lookForResources()
{
	//std::cout << "begin move for agent: " << this << std::endl;
	Engine::Point2D<int> newPosition;

	// looking for cells at vision distance
	int value = _world->getValue("resources", _position);

	std::vector<Engine::Point2D<int> > possiblePositions;
	possiblePositions.push_back(_position);

	/* TODO :
	does it matters how you explore space?
	1.-spiral
	2.-random walk
	3.-nearest peak, scan/look, explore
	....
	here the agent is at <x,y> and scans from <x-h,y-h> to <x+h,y+h>. Do people act like that?
	*/
	for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)
		{
			if(_world->getOverlapBoundaries().isInside(newPosition))
			{
				int valueAtNewPosition = _world->getValue("resources", newPosition);
				// TODO
				// value(cell_i_j) = f(resources_i_j, slope_i_j, terrainType_i_j,....)
				if(valueAtNewPosition>value)	// new position better than current
				{
					possiblePositions.clear();
					possiblePositions.push_back(newPosition);
					value = valueAtNewPosition;
				}
				// in case of being equal target and new position, check distance
				else if(valueAtNewPosition==value)
				{
					possiblePositions.push_back(newPosition);
				}
			}
		}
	}

	if(possiblePositions.size()==0)
	{
	// LOG; A well constructed World will not arrive a state that will
	// conduct you here.
	// You will arrive here if bad dimensioning happens.
		std::stringstream oss;
		oss << "NeandertalGroup::move: agent: " << this << " doesn't have any valid position";
		throw Engine::Exception(oss.str());
	}
	
	std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	// TODO
	// does moving consumes any energy???
	// it should so.
	_position = possiblePositions[0];
}

void NeandertalGroup::lookForPreys( Engine::World::AgentsList & preys )
{
	Engine::Point2D<int> newPosition;

	int value = 0;
	
	// we will move to the zone with more suitable preys around it
	// TODO neard is a dangerous prey... why not hunt lonely preys?
	// when you attack preys they could kill you...
	std::vector<Engine::Point2D<int> > possiblePositions;
	for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)	
		{
			if(_world->getOverlapBoundaries().isInside(newPosition))
			{								//TODO         |----------- allow hierarchy for labels
				int valueAtNewPosition = _world->countNeighbours(this, _homeRange, "neandertal");
				// if has more neighbors than target
				if(valueAtNewPosition>value)
				{
					possiblePositions.clear();
					possiblePositions.push_back(newPosition);
					value = valueAtNewPosition;
				}
				// in case of being equal target and new position, check distance
				else if(valueAtNewPosition==value)
				{
					possiblePositions.push_back(newPosition);
				}
			}
		}
	}
	if(value==0)
	{
		return;
	}
	std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	_position = possiblePositions[0];

	for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)
		{
			Agent * agent = _world->getAgent(newPosition);
			if(agent && agent!=this && agent->exists() && agent->isType("neandertal"))
			{
				preys.push_back(agent);
				// TODO agent = pack of neards
				// but lookout, if agent = 1 neard than you are allowing
				// preying on your partners !!! 
			}
		}
	}
}
//--------------------------------------------------
void NeandertalGroup::offspringAtStep()
{
	if (_resources >= _minForkResources)
	    {
	    Engine::Point2D<int> index = _position;
	    Engine::Point2D<int> neighIndex;
	    // Evaluate surroundings. 
	    // Count free space for offspring
	    int freecells = 0;
	    for(neighIndex._x = index._x-1; neighIndex._x <= index._x+1; neighIndex._x++)
		for(neighIndex._y = index._y-1; neighIndex._y <= index._y+1; neighIndex._y++)
		    {
		    if ((neighIndex._x != index._x || neighIndex._y != index._y) 
		    && _world->getOverlapBoundaries().isInside(neighIndex) 
		    && _world->checkPosition(neighIndex))
			{
			// TODO ensure _allowMultipleAgentsPerCell == false
			//_world->getPopulation(neighIndex).size() == 0, ensured by checkPosition
			freecells++;
			}
		    }
	    if ( freecells > 0 )
		{
		// Throw dice to allocate offspring
		for(neighIndex._x = index._x-1; neighIndex._x <= index._x+1; neighIndex._x++)
		    for(neighIndex._y = index._y-1; neighIndex._y <= index._y+1; neighIndex._y++)
		    {
		    if ((neighIndex._x != index._x || neighIndex._y != index._y) 
		    && _world->getOverlapBoundaries().isInside(neighIndex) 
		    && _world->checkPosition(neighIndex))
			{
			if (_world->getStatistics().getUniformDistValue(1,freecells) == 1)
				{
				// The dice has chosen current cell at neighIndex. Jump out 
				// of the loop.
				// TODO what about using a "dup()" method??
				_resources = _resources - _splitWaste;
				_resources = _resources / 2;
				std::ostringstream oss;
				oss << "neandertal_" << ((NEDWorld*)_world)->getId() << "-" << ((NEDWorld*)_world)->getNewKey();
				NeandertalGroup * child = new NeandertalGroup(oss.str()
									, neighIndex
									, true
									, _world
									, _resources
									, _population
									, _homeRange
									, _cannibalism
									, 0
									, 0
									, _splitChance
									, _children
									, _minForkResources
									, _splitWaste
									, _maxAge
									, _minFoodToEat
									, _minFoodToMove);
				 
				std::cout << _world->getSimulation().getId() << " new neandertal: " << child << std::endl;
				_world->addAgent(child); // TODO is it correct to do the add here? or must world call offspringStep over the agent
							 // and receive a list of agent offspring to add???
				goto endOffspringing;
				// The MISRA C++ coding standard allows the use of goto to 
				// cover this exact kind of situation : nested loops
				}
			else 
				{
				freecells--; 
				// For each iteration where I do not select an item I increase probability,
				// this ensures that you will not end choosing none of the cells.
				// In the last iteration freecells = 1, and this will ensure a cell will be chosen.
				// The procedure gives the same probability 1/freecells for each cell.
				}    
			}
		    }
		endOffspringing:;
		}    

// TODO efficiency : reimplement Split : int s=0; $forall a:Agent: if (| a.pos - this._pos | <= 1) {s++;} freecells = 8 - s; 

	    }

}
//--------------------------------------------------
void NeandertalGroup::death()
{
	if(checkDeath())
	  {
	  _exists = false;
	  _world->removeAgent(this);
	  //return;
	  }	
}
//--------------------------------------------------
void NeandertalGroup::eat()
{
	if (_resources < _minFoodToEat )
	    {
	    // eat
	    int value = _world->getValue("resources", _position);
	    if ( value < _minFoodToMove )
		{
		//scan
		lookForResources();
		//move
		// integrated inside "lookForResources"
		}
	    else collectResources();	
	    }
// TODO: when I move, is it allowed to eat in the same time step?
}
//--------------------------------------------------
void NeandertalGroup::consumption()
{
	_resources = _resources - 1;
}
//--------------------------------------------------
void NeandertalGroup::step()
{
	if(!_exists)
	{
	_world->removeAgent(this);
	return;
	}

	Engine::Point2D<int> oldPos = _position;
	_age++;

//TODO: which order must follow the actions? random?

	// Consumption -------
	consumption();
	// Split -------------
	offspringAtStep();
	// Death -------------
	death();
	// Eat ---------------
	eat();
	// ... ---------------
    
	//TODO RETOCA o REUTILITZA adjustPopulation();only when an agent represents 2 or more individuals
	
	// checkSplit(); only when an agent represents 2 or more individuals
	
//	std::cout << "STEP, beginning at: " << oldPos << " and now: " << this << " with pop: "<< _population << std::endl;
}
//--------------------------------------------------
void NeandertalGroup::collectResources()
{
	Engine::Point2D<int> location;
	for(location._x=_position._x-_homeRange; location._x<=_position._x+_homeRange; location._x++)
	{
		for(location._y=_position._y-_homeRange; location._y<=_position._y+_homeRange; location._y++)
		{
			Agent * agent = _world->getAgent(location);
			if(_world->getOverlapBoundaries().isInside(location) && (!agent || !agent->exists() || agent==this))
			{
				int actualValue = _world->getValue("resources", location);
				//int collected = actualValue;
				// TODO watch your collect capacity / throughput 
				// depending on group size and your competence
				int collected = _world->getStatistics().getNormalDistValue(1,actualValue); // NormalDistValue = competence??
				_resources += collected;
				_world->setValue("resources", location, actualValue-collected);
			}
		}
	}
}
//--------------------------------------------------
void NeandertalGroup::cannibalize( NeandertalGroup * prey )
{
	//std::cout << this << " has prey: " << prey << " with pop: " << prey->getPopulation() << std::endl;;
	int maxPopulation = prey->getPopulation();
	// TODO int hunted = getUniformDistValue(0,maxPopulation);
	// TODO you can hunt some of them, but you can loose people from your band also!!!
	int hunted = _world->getStatistics().getNormalDistValue(1,maxPopulation);
	prey->setPopulation(maxPopulation-hunted);
	if(prey->checkDeath())
	{
		// TODO if it is an overlap agent, it won't be removed...
		prey->setExists(false);
		//removeAgent(prey);
	}
	//std::cout << " modified to: " << prey << " with new pop: " << prey->getPopulation() << std::endl;
	//interactionWithAgent(prey);
	for(int i=0; i<hunted; i++)
	{
		_resources += _world->getStatistics().getNormalDistValue(0,5); // between 0 and 5, just like a cell		
	}
	//std::cout << this << " updating cannibal have collected resources: " << _resources << std::endl;
	//_environment->getRaster("cannibalized").setResource(localPos, _resources);
	//std::cout << "ned group: " << _id << " done" << std::endl;
	_stepsUsingCannibalism++;
}

void NeandertalGroup::adjustPopulation()
{
//	float surplus = _resources - _homeRange*_population;
	int surplus = _resources - _population;
	// we remove 3 resources for each population, and divide in 3 because every new agent will consume 3 next stage
//	surplus /= (float)_homeRange;
//	std::cout << _id << " població inicial: " << _population << " i recursos: " << _resources << " and surplus: " << surplus << std::endl;

	if(std::abs(surplus)<0.5f)
	{
//		std::cout << _id << " surplus 0, població mantinguda a: " << _population << std::endl;
		return;
	}
	if(surplus<0)
	{
		surplus = -surplus;
		surplus = floor(surplus+0.5f);
		_population -= _world->getStatistics().getNormalDistValue(0,surplus);
//		std::cout << _id << " surplus negatiu: " << surplus << ", població reduïda a: " << _population << std::endl;
	}
	else
	{
		surplus = floor(surplus+0.5f);
		_population += _world->getStatistics().getNormalDistValue(0,surplus);
//		std::cout << _id << " surplus positiu: " << surplus << ", població augmentada a: " << _population << std::endl;
	}

}


bool NeandertalGroup::starvation()
{
return _resources <= 0; // TODO change 0 by minResourcesToLive
}


bool NeandertalGroup::checkDeath()
{
	return (_population<1 || starvation() || _age > _maxAge);
}

void NeandertalGroup::checkSplit()
{
	if(_population<2)
	{
		return;
	}
	if(_world->getStatistics().getUniformDistValue(0,10000)<pow((double)(_population),(double)_splitChance))
	{
		// we need to divide population between both groups, 1 + [0..population-2]
		//std::cout << _id << " split achieved with population: " << _population << " and split chance: " << _splitChance << " and value: " << value << std::endl;
		int splittingPopulation = _world->getStatistics().getUniformDistValue(1, _population-1);
		Engine::Point2D<int> splitLocation = getSplitLocation();
		if(_world->getOverlapBoundaries().isInside(splitLocation))
		{
			NEDWorld * world = (NEDWorld*)(_world);
			std::ostringstream oss;
			oss << "neandertal_" << world->getId() << "-" << world->getNewKey();
			NeandertalGroup * splitGroup = new NeandertalGroup(oss.str());
			splitGroup->setPosition(splitLocation);
			_world->addAgent(splitGroup);
			_population = _population - splittingPopulation;
			splitGroup->setPopulation(splittingPopulation);
				
			splitGroup->setCannibalism(_cannibalism);

			/*
			// some flexibility in the cultural trait, 10% of mutation
			if(_world->getStatistics().getUniformDistValue(0,10)==0)
			{
				splitGroup->setCannibalism(std::max(0,_cannibalism-1));
			}
			else if(_world->getStatistics().getUniformDistValue(0,10)==0)
			{
				splitGroup->setCannibalism(std::min(10,_cannibalism+1));
			}
			else
			{
				splitGroup->setCannibalism(_cannibalism);
			}
			*/
			splitGroup->setSplitChance(_splitChance);
			splitGroup->setHomeRange(_homeRange);
			_children++;
		}
	}
}

Engine::Point2D<int> NeandertalGroup::getSplitLocation()
//Engine::Point2D<int> NeandertalGroup::getSplitLocation( int minimumResources )
{
	std::vector<Engine::Point2D<int> > possiblePositions;
	// TODO this method excludes the creation of children in cells in other environments
	Engine::Point2D<int> location = _position;

	for(location._x=_position._x-_homeRange; location._x<=_position._x+_homeRange; location._x++)
	{
		for(location._y=_position._y-_homeRange; location._y<=_position._y+_homeRange; location._y++)
		{
			if(_world->getOverlapBoundaries().isInside(location) && _world->checkPosition(location))
			{
				possiblePositions.push_back(location);
				// the splitting is only possible if the future position is richer enough to maintain the group
				//if(_environment->getRaster("resources").getResource(localPosition)>=minimumResources)
				//{}
			}
		}
	}
	if(possiblePositions.size()==0)
	{
		return Engine::Point2D<int>(-1,-1);
	}
	else
	{
		std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
		return possiblePositions[0];
	}
}

void NeandertalGroup::setHomeRange( const int & homeRange )
{
	_homeRange = homeRange;
}

void NeandertalGroup::setSplitChance( const int & spliChance )
{
	_splitChance = spliChance;
}

void NeandertalGroup::setCannibalism( const int & cannibalism )
{
	_cannibalism = cannibalism;
}

void NeandertalGroup::setResources( const int & resources )
{
	_resources = resources;
}



const int & NeandertalGroup::getPopulation()
{
	return _population;
}

void NeandertalGroup::setPopulation( const int & population )
{
	_population = population;
}

void NeandertalGroup::serialize()
{
	serializeAttribute("population", _population);
	serializeAttribute("home range", _homeRange);
	serializeAttribute("resources gathered", _resources);
	serializeAttribute("cannibalism", _cannibalism);
	serializeAttribute("age", _age);
	serializeAttribute("split chance", _splitChance);
	serializeAttribute("steps using cannibalism", _stepsUsingCannibalism);
	if(_age>0)
	{
		int value = _stepsUsingCannibalism*100;
		value /= _age;
		serializeAttribute("cannibalism age", value);
	}
	else
	{
		serializeAttribute("cannibalism age", 0);
	}
	serializeAttribute("children", _children);
}

void * NeandertalGroup::createPackage()
{
	NeandertalGroupPackage * package = new NeandertalGroupPackage;
	memcpy(&package->_id, _id.c_str(), sizeof(char)*_id.size());
	package->_id[_id.size()] = '\0';

	package->_position = _position;
	package->_exists = _exists;

	package->_resources = _resources;
	package->_population = _population;
	package->_homeRange = _homeRange;
	package->_cannibalism = _cannibalism;

	package->_stepsUsingCannibalism = _stepsUsingCannibalism;
	package->_age = _age;
	package->_splitChance = _splitChance;
	package->_children = _children;
	
	return package;
}
/*
std::ostream & operator<<( std::ostream & stream, NeandertalGroup & agent )
{
	if(agent.getWorld())
	{
		return stream << "id: " << agent.getId() << " pos: " << agent.getPosition() << " population: " << agent.getPopulation() << " and cannibalism: " << agent.getCannibalism() << " exists: " << agent.exists() << " at env: " << agent.getEnvironment() << " located at: " << agent.getEnvironment()->getPosition();
	}
	else
	{
		return stream << "id: " << agent.getId() << " pos: " << agent.getPosition();
	}
}

std::ostream & operator<<( std::ostream & stream, NeandertalGroup * agent )
{
	if(agent->getWorld())
	{
		return stream << "id: " << agent->getId() << " pos: " << agent->getPosition() <<  " population: " << agent->getPopulation() << " and cannibalism: " << agent->getCannibalism() << " exists: " << agent->exists();
	}
	else
	{
		return stream << "id: " << agent->getId() << " pos: " << agent->getPosition() << " exists: " << agent->exists();
	}
}
*/

} // namespace NED

