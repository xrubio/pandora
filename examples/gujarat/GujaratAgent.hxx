
#ifndef __GujaratAgent_hxx__
#define __GujaratAgent_hxx__

#include <Agent.hxx>

#include <vector>
#include <list>

namespace Gujarat
{

class GujaratAgent : public Engine::Agent
{
protected:
	// father = 0, mother = 1, children > 1
	std::vector<int> _populationAges; // MpiVectorAttribute
private:
	void updateAges();
	void checkReproduction();
	void checkMortality();
	void checkAgentRemoval();
	void checkMarriage();
	void checkStarvationMortality();

	GujaratAgent * getMarriageCandidate();

	virtual GujaratAgent * createNewAgent() = 0;

protected:
	int _availableTime; // MpiBasicAttribute
	int _spentTime; // MpiBasicAttribute
	int _collectedResources; // MpiBasicAttribute

	// age of the agent in num steps (years*3)
	int _age; // MpiBasicAttribute

	// allowed range for social interaction
	int _socialRange; // MpiBasicAttribute

	int _homeMobilityRange; // MpiBasicAttribute

	float _massToCaloriesRate; // MpiBasicAttribute

	float _walkingSpeedHour; // MpiBasicAttribute
	float _forageTimeCost; // MpiBasicAttribute
	float _availableForageTime; // MpiBasicAttribute

	float _starved; // MpiBasicAttribute
	
	Engine::Point2D<int> getNearLocation( int range );
	int _forageActionsExecuted;
	int _moveHomeActionsExecuted;

public:
	GujaratAgent( const std::string & id );
	virtual ~GujaratAgent();

	void	setSocialRange( int v ) { _socialRange = v; }
	int	getSocialRange() const { return _socialRange; }
	void	setHomeMobilityRange( int v ) { _homeMobilityRange = v; }
	int	getHomeMobilityRange( ) const { return _homeMobilityRange; } 
	
	void	setMassToCaloriesRate( float v ) { _massToCaloriesRate = v; }

	void	initializePosition( );

	int	getNrAvailableAdults() const;
	int	getNrChildren() const;
	int	getPopulationSize() const;

	// MRJ: Checks if agent member dies with chance%, with age in [min,max]
	void	checkDeath( int minAge, int maxAge, float chance );
	// if male or female died, reproduction is impossible	
	bool	canReproduce() const;
	void	addNewIndividual( int age );
	void	addNewChild();

	int	getOnHandResources() const { return _collectedResources; }
	virtual int	computeConsumedResources( int timeSteps ) const;
	// if fullPopulation = false, only half the individuals will contribute to the action (the rest is moving home)
	float computeMaxForagingDistance( bool fullPopulation = true ) const;
	int	computeEffectiveBiomassForaged( int nominal ) const;
	int	convertBiomassToCalories( int biomass ) const;
	
    void	setWalkingSpeedHour( float v ) { _walkingSpeedHour = v; }
	float	getWalkingSpeedHour() const { return _walkingSpeedHour; }

	void	setForageTimeCost( float v ) { _forageTimeCost = v; }
	float	getForageTimeCost() const { return _forageTimeCost; }

	float getAvailableTime() const;

	double	getTimeSpentForagingTile() const;

	void logAgentState();
	void updateKnowledge();
	void updateState();

	// this method sets the population of the agent to a reasonable starting point
	// in particular, both parents will have ages between 15 and 50
	// taking into account the minimum age of parents a set of children is generated following the rules of the system
	void createInitialPopulation( int adulthoodAge );
	float getPercentageOfStarvingDays() const;
	int getStarved() const {return _starved;}
	
	void incrementMoveHomeActionsExecuted(){_moveHomeActionsExecuted++;}
	void incrementForageActionsExecuted(){_forageActionsExecuted++;}

};

} // namespace Gujarat

#endif // __GujaratAgent_hxx__

