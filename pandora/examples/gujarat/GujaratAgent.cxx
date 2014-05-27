
#include <GujaratAgent.hxx>

#include <GujaratWorld.hxx>
#include <GujaratDemographics.hxx>
#include <CaloricRequirementsTable.hxx>
#include <GujaratConfig.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <GujaratState.hxx>
#include <sstream>
#include <Exceptions.hxx>

namespace Gujarat
{

GujaratAgent::GujaratAgent( const std::string & id ) 
	: Engine::Agent(id)
	, _spentTime(0)
	, _collectedResources(0)
	, _age(0)
	, _socialRange( 50 )
	, _starved( 0.0f )
	, _forageActionsExecuted(0)
	, _moveHomeActionsExecuted(0)
	, _lastMoveCrono(-1)
{
	//_emigrationProbability = 0.0;
	//_reproductionProbability = 0.0;
}

GujaratAgent::~GujaratAgent()
{
}

int	GujaratAgent::convertBiomassToCalories( int biomass ) const
{
	float fMass = (float)biomass;
	return (int)(fMass* _massToCaloriesRate);	
}

int	GujaratAgent::computeEffectiveBiomassForaged( int nominal ) const
{
	
	return Engine::GeneralState::statistics().getNormalDistValue(0, nominal);
	
}

void GujaratAgent::updateKnowledge()
{
}

void GujaratAgent::logAgentState()
{
	std::stringstream logName;
	logName << "agents_" << _world->getId() << "_" << getId();

	log_DEBUG( logName.str(), "timestep=" << getWorld()->getCurrentTimeStep());
	log_DEBUG( logName.str(), "\tagent.collectedResourcesBeforeAction=" << getOnHandResources());
	log_DEBUG( logName.str(), "\tagent.nrAdults=" << getNrAvailableAdults());
	log_DEBUG( logName.str(), "\tagent.adultAges=[");

	for ( unsigned k = 0; k < _populationAges.size(); k++ )
	{
		if ( _populationAges[k] >= ((GujaratWorld*)_world)->getConfig()._adulthoodAge )
		{
			log_DEBUG( logName.str(),  "\t" << _populationAges[k] );
		}
	}
	log_DEBUG( logName.str(), "\t]");
	
	log_DEBUG( logName.str(), "\tagent.nrChildren=" << getNrChildren() );
	log_DEBUG( logName.str(),"\tagent.childrenAges=[");
	for ( unsigned k = 0; k < _populationAges.size(); k++ )
	{
		if ( _populationAges[k] >= 0 && _populationAges[k] < ((GujaratWorld*)_world)->getConfig()._adulthoodAge )
		{
			log_DEBUG( logName.str(), "\t" << _populationAges[k]);
		}
	}
	log_DEBUG( logName.str(), "\t]");

	log_DEBUG( logName.str(), "\tagent.resourcesNeeded=" << computeConsumedResources(1));
}

void GujaratAgent::updateState()
{
	std::stringstream logName;
	logName << "agents_" << _world->getId();

//	log_DEBUG( logName.str(), "\tagent.collectedResourcesAfterAction=" << getOnHandResources());
	int surplus = _collectedResources - computeConsumedResources(1);
//	log_DEBUG( logName.str(), "\tagent.collectedResourcesAfterConsumption=" << surplus);
	if ( surplus < 0 )
	{
        float starvedIncrease = 1.0f-((float)_collectedResources/(float)(computeConsumedResources(1)));
        if(starvedIncrease<0.0f)
        {
			std::stringstream oss;
			oss << "GujaratAgent::updateState - starved increase: " << starvedIncrease << " is negative, collected: " << _collectedResources << " needed: " << computeConsumedResources(1);
			throw Engine::Exception(oss.str());
            return;
        }
		_starved += starvedIncrease;

		//_emigrationProbability += 1.0f/(float)(((GujaratWorld*)_world)->getConfig()._daysPerSeason);
		//log_DEBUG( logName.str(),  "\tagent.isStarving=yes");
	}
	else
	{
		//log_DEBUG( logName.str(), "\tagent.isStarving=no");
		//_reproductionProbability += 1.0/(float)(3*((GujaratWorld*)_world)->getConfig()._daysPerSeason);
		// Decay factor, modeling spoilage
		//_collectedResources *= getSurplusSpoilageFactor();
	}


	log_INFO(logName.str(), this << " starved: " << _starved << " with available time: " << getAvailableTime() << " collected: " << _collectedResources << " and needed: " << computeConsumedResources(1));

	/*
	if ( (getWorld()->getCurrentTimeStep() % ((GujaratWorld*)_world)->getConfig()._daysPerSeason == 0) 
		&& (getWorld()->getCurrentTimeStep() > ((GujaratWorld*)_world)->getConfig()._daysPerSeason-1) ) 
	{
		if( checkEmigration() )
		{
			log_DEBUG( logName.str(), "\tagent.emigration=yes");
			_world->removeAgent(this);
			return;
		}
	}
	*/
	
	// end of year, evaluate reproduction, mortality and update age
	if((getWorld()->getCurrentTimeStep() % ((GujaratWorld*)_world)->getConfig()._daysPerYear == ((GujaratWorld*)_world)->getConfig()._daysPerYear -1))
	{
		updateAges();
		checkMortality();
		checkReproduction();
		checkMarriage();
		checkAgentRemoval();
		
		int numChildren = 0;
		//std::cout << "agent: " << this << " with male : " << _populationAges.at(0) << " female: " << _populationAges.at(1) << " -- children: ";
		for(unsigned int i=2; i<_populationAges.size(); i++)
		{
			if(_populationAges.at(i)!=-1)
			{
				numChildren++;
				//std::cout << "[" << i << "]=" << _populationAges.at(i) << " - ";
			}
		}
		//std::cout << " total: " << numChildren << std::endl;
		_starved = 0;
	}
}

/*
bool	GujaratAgent::checkEmigration()
{
	return GujaratState::demographics().checkEmigration(*this);

}
*/

/*
void	GujaratAgent::checkIndividualStarvationDeath(int index, int deficitRatioPer1000)
{
	int probabSurvPer10000;
	
	if (_populationAges[index] < 15)
	{	probabSurvPer10000= 9600; }//9885=20 days survival; // 9600=10 days survival	
	else
	{	probabSurvPer10000= 9966; }//9966=40 days survival
	
	if(_populationAges[index] >= 0 
		&& Engine::GeneralState::statistics().getUniformDistValue(1,1000) > deficitRatioPer1000
		&& Engine::GeneralState::statistics().getUniformDistValue(1,10000) > probabSurvPer10000)
	{
		//log() << "RIP/Starvation> Agent_"	<< _agent._id << " indiv:" << index << " ratio:" 				<< deficitRatioPer1000 << " age:" 					<< _agent._populationAges[index] << std::endl;
		_populationAges[index]=-1;
	}
}
*/


double 	GujaratAgent::getTimeSpentForagingTile() const
{
	return getForageTimeCost() * getWalkingSpeedHour();
}

float GujaratAgent::computeMaxForagingDistance( bool fullPopulation ) const
{
	
	float value = getWalkingSpeedHour() * getAvailableTime();

	if(!fullPopulation)
	{
		return value/2;
	}
	return value;
}

float GujaratAgent::getAvailableTime() const
{
	float result = 0.0f;
	for(unsigned int i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges.at(i)==-1)
		{
			continue;
		}
		result += GujaratState::availableForageTime(getType(), _populationAges.at(i));
		//std::cout << "age: " << _populationAges.at(i) << " available time: " << GujaratState::availableForageTime(getType(), _populationAges.at(i)) << std::endl;
	}
	//std::cout << "available time for agent: " << this << " is: " << result << std::endl;
	return result;
}

GujaratAgent * GujaratAgent::getMarriageCandidate()
{
	Engine::World::AgentsVector possible;
	Engine::World::AgentsVector neighbors = _world->getNeighbours(this, _socialRange, getType());
	for(Engine::World::AgentsVector::iterator it=neighbors.begin(); it!=neighbors.end(); it++)
	{
		GujaratAgent * possibleAgent = (GujaratAgent*)(*it);
		for(unsigned i=2; i<possibleAgent->_populationAges.size(); i++)
		{	
			// it avoids dead children (-1)
			if(possibleAgent->_populationAges[i]>((GujaratWorld*)_world)->getConfig()._adulthoodAge)
			{
				possible.push_back(possibleAgent);
			}
		}
	}

	if(possible.size()==0)
	{
		return 0;
	}
	//std::random_shuffle(possible.begin(), possible.end());
	GujaratAgent * result = (GujaratAgent*)(possible.at(Engine::GeneralState::statistics().getUniformDistValue(0,possible.size()-1)));
	possible.clear();
	neighbors.clear();
	return result;
}

void GujaratAgent::checkMarriage()
{
	for(unsigned i=2; i<_populationAges.size(); i++)
	{
		if(_populationAges[i]>((GujaratWorld*)_world)->getConfig()._adulthoodAge)
		{
			GujaratAgent * agent = getMarriageCandidate();

			if(!agent)
			{
				//std::cout << "no agent near this one with candidate" << std::endl;
				return;
			}
			
			GujaratAgent * newAgent = createNewAgent();

			newAgent->_populationAges[0] = _populationAges[i];
			_populationAges[i] = -1;

			for(unsigned int j=2; j<agent->_populationAges.size(); j++)
			{
				if(agent->_populationAges[j]>((GujaratWorld*)_world)->getConfig()._adulthoodAge)
				{
					newAgent->_populationAges[1] = agent->_populationAges[j];
					agent->_populationAges[j] = -1;
					break;
				}
			}
			// location inside home range of husband family
			_world->addAgent(newAgent);
			
			// Decrease population counter in old LR home cell
			Engine::Point2D<int> LRpos;
			((GujaratWorld*)_world)->worldCell2LowResCell(_position,LRpos);	
			((GujaratWorld*)_world)->setValueLR(eLRPopulation, LRpos, ((GujaratWorld*)_world)->getValueLR(eLRPopulation,LRpos) - 1);	
			// Change position
			setPosition(getNearLocation(getSocialRange()));
			// Increase population counter in new LR home cell
			((GujaratWorld*)_world)->worldCell2LowResCell(getPosition(),LRpos);
			((GujaratWorld*)_world)->setValueLR(eLRPopulation, LRpos, 1 + ((GujaratWorld*)_world)->getValueLR(eLRPopulation,LRpos));	
			//std::cout << "new agent created: " << newAgent << " with husband age: " << newAgent->_populationAges[0] << " and wife age: " << newAgent->_populationAges[1] << std::endl;
		}
	}
}

Engine::Point2D<int> GujaratAgent::getNearLocation( int range )
{
	std::vector<Engine::Point2D<int> > possiblePositions;
	// TODO this method excludes the creation of children in cells in other environments
	Engine::Point2D<int> location = _position;
	for(location._x=_position._x-range; location._x<=_position._x+range; location._x++)
	{
		for(location._y=_position._y-range; location._y<=_position._y+range; location._y++)
		{
			if(	_world->getOverlapBoundaries().isInside(location) && 
				_world->checkPosition(location) && _world->getValue(eSoils, location)==DUNE)
			{
				possiblePositions.push_back(location);
			}
		}
	}

	// no one near the agent
	if(possiblePositions.size()==0)
	{
		return Engine::Point2D<int>(-1,-1);
	}
	//std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	Engine::Point2D<int> result = possiblePositions[Engine::GeneralState::statistics().getUniformDistValue(0,possiblePositions.size()-1)];
	possiblePositions.clear();
	return result;
}


void GujaratAgent::checkAgentRemoval()
{
	if(_populationAges[0] == -1 && _populationAges[1] == -1)
	{
		//std::cout << "agent: " << this << " removed" << std::endl;
		_exists = false;
		_world->removeAgent(this);
		Engine::Point2D<int> LRpos;
		((GujaratWorld*)_world)->worldCell2LowResCell(_position,LRpos);	
		((GujaratWorld*)_world)->setValueLR(eLRPopulation, LRpos, ((GujaratWorld*)_world)->getValueLR(eLRPopulation,LRpos) - 1);	
	}
}

void GujaratAgent::updateAges()
{
	for(unsigned int i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges[i]!=-1)
		{
			_populationAges[i]++;
		}
	}
}

void GujaratAgent::checkReproduction()
{
	GujaratState::demographics().checkReproduction(*this);
}

int GujaratAgent::computeConsumedResources( int timeSteps ) const
{
	int result = 0;
	for(unsigned int i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges.at(i)==-1)
		{
			continue;
		}	
		result += GujaratState::caloricRequirements(getType(), _populationAges.at(i));
		//std::cout << "age: " << _populationAges.at(i) << " requirements: " << GujaratState::caloricRequirements(getType(), _populationAges.at(i)) << std::endl;
	}
	//std::cout << "required resources for agent: " << this << " is: " << result << std::endl;
	//requiredResources += _foodNeedsForReproduction;
	return result * timeSteps;	
}

void GujaratAgent::checkMortality()
{
	GujaratState::demographics().checkMortality(*this);
}


void	GujaratAgent::initializePosition( const Engine::Point2D<int>& initialPosition )
{
	Engine::Point2D<int> index;
	Engine::Point2D<int> LRpos;
	
	const Engine::Rectangle<int>& _boundaries = getWorld()->getBoundaries();
	bool found = false;
	for(index._x=initialPosition._x; !found && index._x<_boundaries._origin._x+_boundaries._size._x; ++index._x)		
	{	
		for(index._y=initialPosition._y; !found && index._y<_boundaries._origin._y+_boundaries._size._y; ++index._y)		
		{
			found = (getWorld()->getValue(eSoils, index ) == DUNE) && (getWorld()->checkPosition(index));
			if(found)
			{
				setPosition( index );
			}
		}
	}	
	
}

void	GujaratAgent::initializePosition( )
{		
	// 1. select settlement area
	GujaratWorld* world = dynamic_cast<GujaratWorld*>(getWorld());	
	std::vector< Engine::Point2D<int> > dunes;
	
	while(dunes.empty())
	{
		
		// apply Point2D<int> World::getRandomPosition();
		
		const  std::vector< Engine::Rectangle<int> >& areas = world->getSettlementAreas()->getAreas();
		assert(areas.size() > 0);
		unsigned die = Engine::GeneralState::statistics().getUniformDistValue(0, areas.size()-1);
		Engine::Rectangle<int> area = areas[die];
		
		
		for ( int x = area._origin._x; x < area._origin._x + area._size._x; x++ )
		{
			for ( int y = area._origin._y; y < area._origin._y + area._size._y; y++ )
			{
				Engine::Point2D<int> p(x,y);
				if ( getWorld()->getValue(eSoils, p ) == DUNE )
				{
					dunes.push_back( p );
				}
			}
		}
		
	}	
	
		
	/*
	if(dunes.empty())
	{
		std::cout << "empty area: " << area << std::endl;
		for ( int x = area._origin._x; x < area._origin._x + area._size._x; x++ )
		{
			for ( int y = area._origin._y; y < area._origin._y + area._size._y; y++ )
			{
				Engine::Point2D<int> p(x,y);
				std::cout << "value: " << getWorld()->getValue("soils", p) << std::endl;
			}
		}
	}
	assert( !dunes.empty() );
	*/
	unsigned die2 = Engine::GeneralState::statistics().getUniformDistValue(0, dunes.size()-1);
	
	// Decrease population counter in old LR home cell
	Engine::Point2D<int> LRpos;
	((GujaratWorld*)_world)->worldCell2LowResCell(_position,LRpos);	
	((GujaratWorld*)_world)->setValueLR(eLRPopulation, LRpos, ((GujaratWorld*)_world)->getValueLR(eLRPopulation,LRpos) - 1);	
	// Change position
	setPosition( dunes[die2] );

	dunes.clear();
}


int	GujaratAgent::getNrAvailableAdults() const
{
	int numAdults = 0;
	for(unsigned i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges[i] >= ((GujaratWorld*)_world)->getConfig()._adulthoodAge)
		{
			numAdults++;
		}
	}
	return numAdults;
}

int	GujaratAgent::getNrChildren() const
{
	int numChildren = 0;
	for(unsigned i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges[i] < ((GujaratWorld*)_world)->getConfig()._adulthoodAge)
		{
			numChildren++;
		}
	}
	return numChildren;
}

int	GujaratAgent::getPopulationSize() const
{
	int popSize = 0;
	for(unsigned int index=0; index<_populationAges.size(); index++)
	{
		if(_populationAges[index]!=-1)
		{
			popSize++;
		}
	}
	return popSize;	
}

void	GujaratAgent::decimatePopulation()
{
	for(unsigned int index=0; index<_populationAges.size(); index++)
	{
		if(_populationAges[index]!=-1)
		{	
			if(Engine::GeneralState::statistics().getUniformDistValue(0,9)==0)
			{
				_populationAges[index]=-1;
			}
		}
	}	
}

void	GujaratAgent::checkDeath( int minAge, int maxAge, float chance )
{
	for( unsigned index = 0; index < _populationAges.size(); index++ )
	{
		if( _populationAges[index] < minAge || _populationAges[index] >= maxAge )
		{
			continue;
		}

		int die = Engine::GeneralState::statistics().getUniformDistValue(0,1000);
		if( die < 10*chance )
		{
			_populationAges[index] = -1;
		}
	}
}

/*
void	GujaratAgent::checkDeathByAging( int minAge )
{
	for ( unsigned index = 0; index < _populationAges.size(); index++ )
	{
		if ( _populationAges[index] <= minAge ) continue;
		int chance = _populationAges[index] - minAge;
		int die = Engine::GeneralState::statistics().getUniformDistValue(0,100);
		if ( die < chance )
			_populationAges[index] = -1;
	}
	
}
*/

bool	GujaratAgent::canReproduce() const
{
	if(_populationAges[0] == -1 || _populationAges[1] == -1)
	{
		return false;
	}
	return true;
}

void 	GujaratAgent::addNewIndividual( int age )
{
	for(unsigned int index=0; index < _populationAges.size(); index++)
	{
		if(_populationAges[index]==-1)
		{
			_populationAges[index] = age;
			return;
		}
	}
	_populationAges.push_back(age);	
	
}

void	GujaratAgent::addNewChild()
{
	addNewIndividual(0);
	//std::cout << this << " have a new baby!" << std::endl;
	// we look for an empty space or add a new one
}

void GujaratAgent::createInitialPopulation( int adulthoodAge )
{
	
	_populationAges.resize(6);
	_populationAges.at(0) = 20;
	_populationAges.at(1) = 20;
	_populationAges.at(2) = 5;
	_populationAges.at(3) = 7;
	_populationAges.at(4) = 10;
	_populationAges.at(5) = 15;

	/*
	_populationAges.at(0) = Engine::GeneralState::statistics().getUniformDistValue(15, 50);
	_populationAges.at(1) = Engine::GeneralState::statistics().getUniformDistValue(15, 50);
	int reproductiveYears = std::min(_populationAges.at(0)-adulthoodAge, _populationAges.at(1)-adulthoodAge);
	// if more than adulthood age, stick to that because other children are already independent
	reproductiveYears = std::min(adulthoodAge,reproductiveYears);

	// we create a set of children, taking into account mortality. It is assumed that parents survived the entire init period
	for(int i=0; i<reproductiveYears; i++)
	{		
		GujaratState::demographics().checkReproduction(*this);	
		GujaratState::demographics().checkMortality(*this);	
		for(unsigned int j=2; j<_populationAges.size(); j++)
		{
			if(_populationAges.at(j) != -1)
			{
				_populationAges.at(j)++;
			}
		}
	}
	
	std::cout << "init pop for agent: " << this << std::endl;
	for(int i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges.at(i)!=-1)
		{
			std::cout << "individual: " << i << " age: " << _populationAges.at(i) << std::endl;
		}
	}
	std::cout << "end init pop for agent: " << this << std::endl;
	*/
}
	
float GujaratAgent::getPercentageOfStarvingDays() const
{
	GujaratWorld * world = (GujaratWorld*)_world;
	return (_starved*100.0f)/float((world->getConfig()._daysPerYear));
}

} // namespace Gujarat

