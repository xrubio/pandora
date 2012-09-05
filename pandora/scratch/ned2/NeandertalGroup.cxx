
#include "NeandertalGroup.hxx"
#include "World.hxx"
#include "Exceptions.hxx"

#include <algorithm>
#include <cstring>

#include "Statistics.hxx"
#include "NEDWorld.hxx"

namespace NED
{

NeandertalGroup::NeandertalGroup( const std::string & id ) : Agent(id), _resources(0), _population(0), _homeRange(0), _cannibalism(0), _stepsUsingCannibalism(0), _age(0), _splitChance(0), _children(0)
{
}

NeandertalGroup::NeandertalGroup( const NeandertalGroupPackage & package ) : Agent(package._id)														
{
	_position = package._position;
	
	_resources = package._resources;
	_exists = package._exists;
	_population = package._population;
	_homeRange = package._homeRange;
	_cannibalism = package._cannibalism;

	_stepsUsingCannibalism = package._stepsUsingCannibalism;
	_age = package._age;
	_splitChance = package._splitChance;
	_children = package._children;
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
		oss << "NeandertalGroup::move: agent: " << this << " doesn't have any valid position";
		throw Engine::Exception(oss.str());
	}
	
	std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	_position = possiblePositions[0];
}

void NeandertalGroup::lookForPreys( Engine::World::AgentsList & preys )
{
	Engine::Point2D<int> newPosition;

	int value = 0;
	
	// we will move to the zone with more suitable preys around it
	std::vector<Engine::Point2D<int> > possiblePositions;
	for(newPosition._x=_position._x-_homeRange; newPosition._x<=_position._x+_homeRange; newPosition._x++)
	{
		for(newPosition._y=_position._y-_homeRange; newPosition._y<=_position._y+_homeRange; newPosition._y++)	
		{
			if(_world->getOverlapBoundaries().isInside(newPosition))
			{
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
			}
		}
	}
}

void NeandertalGroup::step()
{
	if(!_exists)
	{
		_world->removeAgent(this);
		return;
	}
//	std::cout << "BEGIN STEP: " << this << " with population: " << _population <<  std::endl;

	Engine::Point2D<int> oldPos = _position;
	_resources = 0;
	_age++;	
	
	lookForResources();
	collectResources();

	if(_resources<_population)
	{
		if(_cannibalism<10 && _world->getStatistics().getUniformDistValue(0,9)==0)
		{
			_cannibalism++;
		}
		if(_world->getStatistics().getUniformDistValue(1,10)<_cannibalism)
		{
			Engine::World::AgentsList preys;
			lookForPreys(preys);
			if(preys.size()!=0)
			{
				for(Engine::World::AgentsList::iterator it=preys.begin(); it!=preys.end(); it++)
				{
					NeandertalGroup * prey = (NeandertalGroup*)(*it);
					cannibalize(prey);
				}
			}
		}
	}
	else
	{
		if(_cannibalism>0 && (_world->getStatistics().getUniformDistValue(0,9)==0))
		{
			_cannibalism--;
		}
	}
	/*
	if((_resources<_population) && (_world->getStatistics().getUniformDistValue(1,10)<_cannibalism))
	{
		Engine::World::AgentsList preys;
		lookForPreys(preys);
		if(preys.size()!=0)
		{
			for(Engine::World::AgentsList::iterator it=preys.begin(); it!=preys.end(); it++)
			{
				NeandertalGroup * prey = (NeandertalGroup*)(*it);
				cannibalize(prey);
			}
		}
	}*/
/*
	if(_world->getStatistics().getUniformDistValue(1,10)<_cannibalism)
	{
		Engine::World::AgentsList preys;
		lookForPreys(preys);
//		std::cout << this << " will predate: " << preys.size() << " groups" << std::endl;
		if(preys.size()!=0)
		{
			for(Engine::World::AgentsList::iterator it=preys.begin(); it!=preys.end(); it++)
			{
				//if(_world->getStatistics().getUniformDistValue(0,1)==0)
				//{
					NeandertalGroup * prey = (NeandertalGroup*)(*it);
					//std::cout << "prey of: " <<this << " is: " << prey << std::endl;
					cannibalize(prey);
				//}
			}
		}
		else
		{
		//	std::cout << this << " didn't find any prey" << std::endl;
			//lookForResources();
			collectResources();
		}
	}
	else
	{
		lookForResources();
		collectResources();
	}
	*/
/*
	lookForResources();
	collectResources();

	if((_resources<_population) && (Engine::Statistics::instance()->getUniformDistValue(1,10)<_cannibalism))
	{
		NeandertalGroup * prey = lookForPrey(false);
		if(prey)
		{
			cannibalize(prey);
		}
	}
*/
	adjustPopulation();
	if(checkDeath())
	{
		_exists = false;
		_world->removeAgent(this);
		return;
	}
	checkSplit();
//	std::cout << "STEP, beginning at: " << oldPos << " and now: " << this << " with pop: "<< _population << std::endl;
}

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
				int collected = _world->getStatistics().getNormalDistValue(0,actualValue);
				_resources += collected;
				_world->setValue("resources", location, actualValue-collected);
			}
		}
	}
}

void NeandertalGroup::cannibalize( NeandertalGroup * prey )
{
	//std::cout << this << " has prey: " << prey << " with pop: " << prey->getPopulation() << std::endl;;
	int maxPopulation = prey->getPopulation();
	// TODO int hunted = getUniformDistValue(0,maxPopulation);
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

bool NeandertalGroup::checkDeath()
{
	if(_population<1)
	{
		return true;
	}
	return false;
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
		// TODO, PROPOSAL:
		/*
		return possiblePositions[_world->getStatistics().getUniformDistValue(0,possiblePositions.size()-1)];
		*/
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

