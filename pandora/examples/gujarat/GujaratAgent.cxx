
#include <GujaratAgent.hxx>

#include <GujaratWorld.hxx>
#include <GujaratDemographics.hxx>
#include <CaloricRequirementsTable.hxx>
#include <GujaratConfig.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <GujaratState.hxx>
#include <sstream>

namespace Gujarat
{

GujaratAgent::GujaratAgent( const std::string & id ) 
	: Engine::Agent(id), 
	 _spentTime(0), _collectedResources(0), _age(0),
	_socialRange( 50 ), _starved( 0 )
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
	logName << "agents_" << _world->getId() << "_" << getId();

	log_DEBUG( logName.str(), "\tagent.collectedResourcesAfterAction=" << getOnHandResources());
	int surplus = _collectedResources - computeConsumedResources(1);
	log_DEBUG( logName.str(), "\tagent.collectedResourcesAfterConsumption=" << surplus);
	if ( surplus < 0 )
	{
		//_emigrationProbability += 1.0f/(float)(((GujaratWorld*)_world)->getConfig()._daysPerSeason);
		log_DEBUG( logName.str(),  "\tagent.isStarving=yes");
		_starved++;
	}
	else
	{
		log_DEBUG( logName.str(), "\tagent.isStarving=no");
		//_reproductionProbability += 1.0/(float)(3*((GujaratWorld*)_world)->getConfig()._daysPerSeason);
		// Decay factor, modeling spoilage
		//_collectedResources *= getSurplusSpoilageFactor();
	}

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

double	GujaratAgent::computeMaxForagingDistance() const
{
	int 	nAdults = getNrAvailableAdults();		

	return  getWalkingSpeedHour() * getAvailableForageTime() * (double)nAdults;
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
	std::random_shuffle(possible.begin(), possible.end());
	return (GujaratAgent*)(possible.at(0));
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
			if(	_world->getOverlapBoundaries().isInside(location) && 
				_world->checkPosition(location) && _world->getValue("soils", location)==DUNE)
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
		//std::cout << "agent: " << this << " removed" << std::endl;
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
	int requiredResources = 0;
	for(unsigned int index=0; index<_populationAges.size(); index++)
	{
		if(_populationAges[index]!=-1)
		{
			CaloricRequirementsTable & table = GujaratState::caloricRequirements(getType());
			requiredResources += table.getCaloriesFor(_populationAges[index]);
		}
	}
	//requiredResources += _foodNeedsForReproduction;
	return requiredResources * timeSteps;	
}

void GujaratAgent::checkMortality()
{
	GujaratState::demographics().checkMortality(*this);
}

void	GujaratAgent::initializePosition( )
{
	// 1. select settlement area
	GujaratWorld* world = dynamic_cast<GujaratWorld*>(getWorld());
	std::vector< Engine::Point2D<int> > dunes;
	while(dunes.empty())
	{
		const  std::vector< Engine::Rectangle<int> >& areas = world->getSettlementAreas()->getAreas();
		unsigned die = Engine::GeneralState::statistics().getUniformDistValue(0, areas.size()-1);
		Engine::Rectangle<int> area = areas[die];
		for ( int x = area._origin._x; x < area._origin._x + area._size._x; x++ )
		{
			for ( int y = area._origin._y; y < area._origin._y + area._size._y; y++ )
			{
				Engine::Point2D<int> p(x,y);
				if ( getWorld()->getValue("soils", p ) == DUNE )
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
	setPosition( dunes[die2] );
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

void	GujaratAgent::checkDeath( int minAge, int maxAge, int chance )
{
	for( unsigned index = 0; index < _populationAges.size(); index++ )
	{
		if( _populationAges[index] < minAge || _populationAges[index] >= maxAge )
		{
			continue;
		}

		int die = Engine::GeneralState::statistics().getUniformDistValue(0,100);
		if( die < chance )
		{
			_populationAges[index] = -1;
		}
	}
}

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

void GujaratAgent::createInitialPopulation()
{
	/*
	_populationAges.resize(4);
	_populationAges[0] = 25;
	_populationAges[1] = 22;
	_populationAges[2] = 5;
	_populationAges[3] = 7;
	*/
	
	_populationAges.resize(2);
	_populationAges.at(0) = Engine::GeneralState::statistics().getUniformDistValue(15, 50);
	_populationAges.at(1) = Engine::GeneralState::statistics().getUniformDistValue(15, 50);
	int reproductiveYears = std::min(_populationAges.at(0), _populationAges.at(1));
	// if more than 15 years, stick to 15 because other children are already independent
	reproductiveYears = std::min(15,reproductiveYears);

	// we create a set of children, taking into account mortality. It is assumed that parents survived the entire init period
	for(int i=0; i<reproductiveYears; i++)
	{		
		GujaratState::demographics().checkReproduction(*this);	
		checkDeath( 0, 3, 10 );
		checkDeath( 3, 15, 2 );
		for(int j=2; j<_populationAges.size(); j++)
		{
			if(_populationAges.at(j) != -1)
			{
				_populationAges.at(j)++;
			}
		}
	}
}
	
float GujaratAgent::getPercentageOfStarvingDays() const
{
	GujaratWorld * world = (GujaratWorld*)_world;
	return float(_starved*100)/float((world->getConfig()._daysPerYear));
}

} // namespace Gujarat

