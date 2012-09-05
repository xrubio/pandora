
#include "SapiensGroup.hxx"
#include "World.hxx"
#include "Exceptions.hxx"

#include <algorithm>
#include <cstring>

#include "Statistics.hxx"
#include "NEDWorld.hxx"

namespace NED
{


SapiensGroup::SapiensGroup( const std::string & id ) 
    : Agent(id)
    , _resources(0)
    , _population(0)
    , _homeRange(0)
    , _age(0)
    , _splitChance(0)
    , _children(0)
{
	//_population =  Engine::Statistics::instance()->getUniformDistValue(1,10);
}

// ? aquí no s'usa la inicialitzacií object_name::object_name(...) : atrib(initialization_value|param)+
SapiensGroup::SapiensGroup( const SapiensGroupPackage & package ) : Agent(package._id)														
{
	_position = package._position;
	
	_resources = package._resources;
	_exists = package._exists;
	_population = package._population;
	_homeRange = package._homeRange;

	_age = package._age;
	_splitChance = package._splitChance;
	_children = package._children;
}

SapiensGroup::~SapiensGroup()
{
}

void SapiensGroup::lookForResources()
{	
	//std::cout << "begin move for agent: " << this << std::endl;
	Engine::Point2D<int> newPosition;

	// looking for cells at vision distance
	int value = _world->getValue("resources", _position);

	std::vector<Engine::Point2D<int> > possiblePositions;
	possiblePositions.push_back(_position);

	for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)
		{
			if(_world->getOverlapBoundaries().isInside(newPosition))
			{
				int valueAtNewPosition = _world->getValue("resources", newPosition);
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

	if(possiblePositions.size()==0)
	{
		std::stringstream oss;
		oss << "SapiensGroup::move: agent: " << this << " doesn't have any valid position";
		throw Engine::Exception(oss.str());
	}
	
	std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	_position = possiblePositions[0];
}

/*
int SapiensGroup::countNeighbours( const Engine::Point2D<int> & position )
{
	int neighbours = 0;
	Engine::Point2D<int> newPosition;
	for(newPosition._x=position._x-_homeRange; newPosition._x<=position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=position._y-_homeRange; newPosition._y<=position._y+_homeRange; newPosition._y++)
		{
			Agent * agent = _environment->getAgent(newPosition);
			if(agent && agent!=this && agent->exists())
			{
				neighbours++;
			}
		}
	}
	return neighbours;
}

SapiensGroup * SapiensGroup::lookForPrey( bool move )
{
	Engine::Point2D<int> newPosition;

	if(move)
	{
		int value = 0;
	
		// we will move to the zone with more suitable preys around it
		std::vector<Engine::Point2D<int> > possiblePositions;
		for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
		{
			for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)
			{
				if(_environment->checkPosition(newPosition))
				{
					int valueAtNewPosition = countNeighbours(newPosition);
					// if has more neighbors than target
					if(valueAtNewPosition>value)
					{
						possiblePositions.clear();
						possiblePositions.resize(0);
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
			return 0;
		}

		std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
		_position = possiblePositions[0];
	}

	// finally we select a prey from the possible targets
	std::vector<Agent *> possiblePreys;
	for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)
		{
			Agent * agent = _environment->getAgent(newPosition);
			if(agent && agent!=this && agent->exists())
			{
				possiblePreys.push_back(agent);
			}
		}
	}

	if(possiblePreys.size()==0)
	{
		return 0;
	}
	std::random_shuffle(possiblePreys.begin(), possiblePreys.end());
	return (SapiensGroup *)(possiblePreys[0]);
}
*/

void SapiensGroup::step()
{
	if(!_exists)
	{
		_world->removeAgent(this);
		return;
	}
	
	Engine::Point2D<int> oldPos = _position;
	_resources = 0;
	_age++;

	lookForResources();
	collectResources();
	
	adjustPopulation();
	if(checkDeath())
	{
		_exists = false;
		_world->removeAgent(this);
		return;
	}
	checkSplit();
	//std::cout << "STEP, beginning at: " << oldPos << " and now: " << this << std::endl;
}

void SapiensGroup::collectResources()
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
				int collected = _world->getStatistics().getNormalDistValue(0,actualValue);
				_resources += collected;
				_world->setValue("resources", location, actualValue-collected);
			}
		}
	}
}
/*
void SapiensGroup::cannibalize( SapiensGroup * prey )
{
	//std::cout << this << " has prey: " << prey << " with pop: " << prey->getPopulation();
	int maxPopulation = prey->getPopulation();
	// TODO int hunted = getUniformDistValue(0,maxPopulation);
	int hunted = Engine::Statistics::instance()->getUniformDistValue(1,maxPopulation);
	prey->setPopulation(maxPopulation-hunted);
	if(prey->checkDeath())
	{
		// TODO if it is an overlap agent, it won't be removed...
		prey->setExists(false);
		//prey->remove();
	}
	//std::cout << " modified to: " << prey << " with new pop: " << prey->getPopulation() << std::endl;
	interactionWithAgent(prey);
	for(int i=0; i<hunted; i++)
	{
		_resources += Engine::Statistics::instance()->getUniformDistValue(0,5); // between 0 and 5, just like a cell
	}
	Engine::Point2D<int> localPos = _environment->getLocalPosition(_position);
	//std::cout << "ned group: " << _id << " updating cannibal with value: " << _resources << " at pos: " << localPos << " with max value: " << _environment->getRaster("cannibalized").getMaxValue(localPos) << " and actual value: " << _environment->getRaster("cannibalized").getResource(localPos) << std::endl;
	//_environment->getRaster("cannibalized").setResource(localPos, _resources);
	//std::cout << "ned group: " << _id << " done" << std::endl;
}
*/

void SapiensGroup::adjustPopulation()
{
	//	float surplus = _resources - _homeRange*_population;
	int surplus = _resources - _population;
	// we remove 3 resources for each population, and divide in 3 because every new agent will consume 3 next stage
//	surplus /= (float)_homeRange;
	//std::cout << _id << " població inicial: " << _population << " i recursos: " << _resources << " and surplus: " << surplus << std::endl;

	if(std::abs(surplus)<0.5f)
	{
		//std::cout << _id << " surplus 0, població mantinguda a: " << _population << std::endl;
		return;
	}
	if(surplus<0)
	{
		surplus = -surplus;
		surplus = floor(surplus+0.5f);
		_population -= _world->getStatistics().getNormalDistValue(0,surplus);
		//std::cout << _id << " surplus negatiu: " << surplus << ", població reduïda a: " << _population << std::endl;
	}
	else
	{
		surplus = floor(surplus+0.5f);
		_population += _world->getStatistics().getNormalDistValue(0,surplus);
		//std::cout << _id << " surplus positiu: " << surplus << ", població augmentada a: " << _population << std::endl;
	}
}

bool SapiensGroup::checkDeath()
{
	if(_population<1)
	{
		return true;
	}
	return false;
}

void SapiensGroup::checkSplit()
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
			std::ostringstream oss;
			NEDWorld * world = (NEDWorld*)(_world);
			oss << "sapiens_" << world->getId() << "-" << world->getNewKey() << std::endl;
			SapiensGroup * splitGroup = new SapiensGroup(oss.str());
			splitGroup->setPosition(splitLocation);
			_world->addAgent(splitGroup);
			_population = _population - splittingPopulation;
			splitGroup->setPopulation(splittingPopulation);
				
			splitGroup->setSplitChance(_splitChance);
			splitGroup->setHomeRange(_homeRange);
			_children++;
		}
	}
}

Engine::Point2D<int> SapiensGroup::getSplitLocation()
//Engine::Point2D<int> SapiensGroup::getSplitLocation( int minimumResources )
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

void SapiensGroup::setHomeRange( const int & homeRange )
{
	_homeRange = homeRange;
}

void SapiensGroup::setSplitChance( const int & splitChance )
{
	_splitChance = splitChance;
}

const int & SapiensGroup::getPopulation()
{
	return _population;
}

void SapiensGroup::setPopulation( const int & population )
{
	_population = population;
}

void SapiensGroup::serialize()
{
	serializeAttribute("population", _population);
	serializeAttribute("home range", _homeRange);
	serializeAttribute("resources gathered", _resources);
	serializeAttribute("age", _age);
	serializeAttribute("split chance", _splitChance);
	serializeAttribute("children", _children);
}

void * SapiensGroup::createPackage()
{
	SapiensGroupPackage * package = new SapiensGroupPackage;
	memcpy(&package->_id, _id.c_str(), sizeof(char)*_id.size());
	package->_id[_id.size()] = '\0';

	package->_position = _position;
	package->_exists = _exists;

	package->_resources = _resources;
	package->_population = _population;
	package->_homeRange = _homeRange;

	package->_age = _age;
	package->_splitChance = _splitChance;
	package->_children = _children;
	
	return package;
}

} // namespace NED

