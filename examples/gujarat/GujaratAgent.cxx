
#include <GujaratAgent.hxx>

#include <GujaratWorld.hxx>
#include <OriginalDemographics.hxx>
#include <GujaratConfig.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <GujaratState.hxx>
#include <sstream>
#include <typedefs.hxx>

namespace Gujarat
{

GujaratAgent::GujaratAgent( const std::string & id ) 
	: Engine::Agent(id), 
	 _spentTime(0), _collectedResources(0), _age(0),
	_socialRange( 50 ), _starved( 0.0f ), _forageActionsExecuted(0), _moveHomeActionsExecuted(0)
{
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
	return Engine::GeneralState::statistics().getNormalDistValueMinMax(0, nominal);
	
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

    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	for ( unsigned k = 0; k < _populationAges.size(); k++ )
	{
		if ( _populationAges[k] >= gujaratConfig._adulthoodAge )
		{
			log_DEBUG( logName.str(),  "\t" << _populationAges[k] );
		}
	}
	log_DEBUG( logName.str(), "\t]");
	
	log_DEBUG( logName.str(), "\tagent.nrChildren=" << getNrChildren() );
	log_DEBUG( logName.str(),"\tagent.childrenAges=[");
	for ( unsigned k = 0; k < _populationAges.size(); k++ )
	{
		if ( _populationAges[k] >= 0 && _populationAges[k] < gujaratConfig._adulthoodAge )
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
	logName << "agents_" << _world->getId() << "_" << getId();

	log_DEBUG( logName.str(), "\tagent.collectedResourcesAfterAction=" << getOnHandResources());
	int surplus = _collectedResources - computeConsumedResources(1);
	log_DEBUG( logName.str(), "\tagent.collectedResourcesAfterConsumption=" << surplus);
	if ( surplus < 0 )
	{
		_starved += 1.0f-((float)_collectedResources/(float)(computeConsumedResources(1)));
		//_emigrationProbability += 1.0f/(float)(((GujaratWorld*)_world)->getConfig()._daysPerSeason);
		log_DEBUG( logName.str(),  "\tagent.isStarving=yes");
	}
	else
	{
		log_DEBUG( logName.str(), "\tagent.isStarving=no");
	}
	
	// end of year, evaluate reproduction, mortality and update age
    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	if((getWorld()->getCurrentTimeStep() % gujaratConfig._daysPerYear) == (gujaratConfig._daysPerYear -1))
	{
		updateAges();
		checkMortality();
		checkReproduction();
		checkMarriage();
		checkAgentRemoval();
		int numChildren = 0;
		std::cout << "agent: " << this << " with male : " << _populationAges.at(0) << " female: " << _populationAges.at(1) << " -- children: ";
		for(int i=2; i<_populationAges.size(); i++)
		{
			if(_populationAges.at(i)!=-1)
			{
				numChildren++;
				std::cout << "[" << i << "]=" << _populationAges.at(i) << " - ";
			}
		}
		std::cout << " total: " << numChildren << std::endl;
		_starved = 0;
	}
}

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
	for(int i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges.at(i)==-1)
		{
			continue;
		}
		result += GujaratState::availableForageTime(getType(), _populationAges.at(i));
	}
	return result;
}

GujaratAgent * GujaratAgent::getMarriageCandidate()
{
    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	Engine::AgentsVector possible;
	Engine::AgentsVector neighbors = _world->getNeighbours(this, _socialRange, getType());
	for(Engine::AgentsVector::iterator it=neighbors.begin(); it!=neighbors.end(); it++)
	{
		GujaratAgent * possibleAgent = (GujaratAgent*)(it->get());
		for(unsigned i=2; i<possibleAgent->_populationAges.size(); i++)
		{	
			// it avoids dead children (-1)
			if(possibleAgent->_populationAges[i]>gujaratConfig._adulthoodAge)
			{
				possible.push_back(*it);
			}
		}
	}

	if(possible.size()==0)
	{
		return 0;
	}
	std::random_shuffle(possible.begin(), possible.end());
    Engine::Agent * agent = possible.at(0).get();
    return (GujaratAgent*)(agent);
}

void GujaratAgent::checkMarriage()
{
    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	for(unsigned i=2; i<_populationAges.size(); i++)
	{
		if(_populationAges[i]>gujaratConfig._adulthoodAge)
		{
			GujaratAgent * agent = getMarriageCandidate();

			if(!agent)
			{
				return;
			}
			
			GujaratAgent * newAgent = createNewAgent();

			newAgent->_populationAges[0] = _populationAges[i];
			_populationAges[i] = -1;

			for(unsigned int j=2; j<agent->_populationAges.size(); j++)
			{
				if(agent->_populationAges[j]>gujaratConfig._adulthoodAge)
				{
					newAgent->_populationAges[1] = agent->_populationAges[j];
					agent->_populationAges[j] = -1;
					break;
				}
			}
			// location inside home range of husband family
			_world->addAgent(newAgent);
			newAgent->setPosition(getNearLocation(getSocialRange()));
			std::cout << "new agent created: " << newAgent << " with husband age: " << newAgent->_populationAges[0] << " and wife age: " << newAgent->_populationAges[1] << std::endl;
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
			if(	_world->getBoundaries().contains(location) && 
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
	std::random_shuffle(possiblePositions.begin(), possiblePositions.end());
	return possiblePositions[0];
}


void GujaratAgent::checkAgentRemoval()
{
	if(_populationAges[0] == -1 && _populationAges[1] == -1)
	{
		_exists = false;
		_world->removeAgent(this);
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
	for(int i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges.at(i)==-1)
		{
			continue;
		}	
		result += GujaratState::caloricRequirements(getType(), _populationAges.at(i));
	}
	return result * timeSteps;	
}

void GujaratAgent::checkMortality()
{
	GujaratState::demographics().checkMortality(*this);
}

void GujaratAgent::initializePosition( )
{
	// 1. select settlement area
	GujaratWorld* world = dynamic_cast<GujaratWorld*>(getWorld());
	std::vector< Engine::Point2D<int> > dunes;
	while(dunes.empty())
	{
		const  std::vector< Engine::Rectangle<int> >& areas = world->getSettlementAreas()->getAreas();
		unsigned die = Engine::GeneralState::statistics().getUniformDistValue(0, areas.size()-1);
		Engine::Rectangle<int> area = areas[die];
		for ( int x = area._origin._x; x < area._origin._x + area._size._width; x++ )
		{
			for ( int y = area._origin._y; y < area._origin._y + area._size._height; y++ )
			{
				Engine::Point2D<int> p(x,y);
				if ( getWorld()->getValue(eSoils, p ) == DUNE )
				{
					dunes.push_back( p );
				}
			}
		}
	}

	unsigned die2 = Engine::GeneralState::statistics().getUniformDistValue(0, dunes.size()-1);
	setPosition( dunes[die2] );
}

int	GujaratAgent::getNrAvailableAdults() const
{
	int numAdults = 0;
    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	for(unsigned i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges[i] >= gujaratConfig._adulthoodAge)
		{
			numAdults++;
		}
	}
	return numAdults;
}

int	GujaratAgent::getNrChildren() const
{
	int numChildren = 0;
    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	for(unsigned i=0; i<_populationAges.size(); i++)
	{
		if(_populationAges[i] < gujaratConfig._adulthoodAge)
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

void GujaratAgent::checkDeath( int minAge, int maxAge, float chance )
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

bool GujaratAgent::canReproduce() const
{
	if(_populationAges[0] == -1 || _populationAges[1] == -1)
	{
		return false;
	}
	return true;
}

void GujaratAgent::addNewIndividual( int age )
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

void GujaratAgent::addNewChild()
{
	addNewIndividual(0);
}

void GujaratAgent::createInitialPopulation( int adulthoodAge )
{
	_populationAges.resize(2);
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
		for(int j=2; j<_populationAges.size(); j++)
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
}
	
float GujaratAgent::getPercentageOfStarvingDays() const
{
    const GujaratConfig & gujaratConfig = (const GujaratConfig &)(getWorld()->getConfig());
	return (_starved*100.0f)/float(gujaratConfig._daysPerYear);
}

} // namespace Gujarat

