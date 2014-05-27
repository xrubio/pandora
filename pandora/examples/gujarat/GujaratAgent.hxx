
#ifndef __GujaratAgent_hxx__
#define __GujaratAgent_hxx__

#include <Agent.hxx>

#include <Sector.hxx>

#include <vector>
#include <list>

#include <assert.h>

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

	//virtual bool checkEmigration();

protected:
	int _availableTime; // MpiBasicAttribute
	int _spentTime; // MpiBasicAttribute
	int _collectedResources; // MpiBasicAttribute
	int _resourcePrediction; // MpiBasicAttribute
	// age of the agent in num steps (years*3)
	int _age; // MpiBasicAttribute

	// allowed range for social interaction
	int _socialRange; // MpiBasicAttribute

	int _homeMobilityRange; // MpiBasicAttribute

	float _massToCaloriesRate;	// MpiBasicAttribute
	//float _surplusSpoilageFactor;
	
	//float   _foodNeedsForReproduction;

	float _walkingSpeedHour; // MpiBasicAttribute
	float _forageTimeCost; // MpiBasicAttribute
	float _availableForageTime; // MpiBasicAttribute
	//float _emigrationProbability; 
	//float _reproductionProbability;

	float _starved; // MpiBasicAttribute
	
	int _lastMoveCrono; // MpiBasicAttribute
	
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
	void    initializePosition( const Engine::Point2D<int>& initialPosition );

	int	getNrAvailableAdults() const;
	int	getNrChildren() const;
	int	getPopulationSize() const;

	// MRJ: Kills 1 out of every 10 people in the agent (on average)
	void	decimatePopulation();
	// MRJ: Checks if agent member dies with chance%, with age in [min,max]
	void	checkDeath( int minAge, int maxAge, float chance );
	//void	checkDeathByAging( int minAge );
	// if male or female died, reproduction is impossible	
	//void	checkIndividualStarvationDeath(int index, int deficitRatioPer1000);
	bool	canReproduce() const;
	void	addNewIndividual( int age );
	void	addNewChild();

	int	getOnHandResources() const { return _collectedResources; }
	virtual int	computeConsumedResources( int timeSteps ) const;
	// if fullPopulation = false, only half the individuals will contribute to the action (the rest is moving home)
	float computeMaxForagingDistance( bool fullPopulation = true ) const;
	int	computeEffectiveBiomassForaged( int nominal ) const;
	int	computeEffectiveBiomassForagedNoStochastic( int nominal ) const
		{ return nominal/2; }
	int	convertBiomassToCalories( int biomass ) const;
//	bool	starvationLastTimeStep() const { return _starved; }

	//void	setSurplusSpoilageFactor( float v ) { _surplusSpoilageFactor = v; }
	//float	getSurplusSpoilageFactor() const { return _surplusSpoilageFactor; }

	//void 	setFoodNeedsForReproduction( float v) { _foodNeedsForReproduction = v; }
	
	void	setWalkingSpeedHour( float v ) { _walkingSpeedHour = v; }
	float	getWalkingSpeedHour() const { return _walkingSpeedHour; }

	void	setForageTimeCost( float v ) { _forageTimeCost = v; }
	float	getForageTimeCost() const { return _forageTimeCost; }

	float getAvailableTime() const;
//	void	setAvailableForageTime( float v ) { _availableForageTime = v; }
//	float	getAvailableForageTime() const { return _availableForageTime; }

	//void	setEmigrationProbability( float v ) { _emigrationProbability = v; }
	//float	getEmigrationProbability() const { return _emigrationProbability; }
	//void	setReproductionProbability( float v ) { _reproductionProbability = v; }
	//float	getReproductionProbability() const { return _reproductionProbability; }

	double	getTimeSpentForagingTile() const;

	void logAgentState();
	void updateKnowledge();
	void updateState();
	virtual void clearSectorKnowledge() = 0;
	//virtual void clearSectorKnowledge(Sector *) = 0;
	virtual void clearSectorKnowledge(Engine::Point2D<int> LRHomeLocationOLD,Engine::Point2D<int> LRHomeLocationNEW) = 0;
	
	virtual void updateKnowledgeFromFirstHandVisit(Sector * s) = 0;	
	
	virtual Engine::Raster & getLRResourcesRaster() = 0;
	
	// this method sets the population of the agent to a reasonable starting point
	// in particular, both parents will have ages between 15 and 50
	// taking into account the minimum age of parents a set of children is generated following the rules of the system
	void createInitialPopulation( int adulthoodAge );
	float getPercentageOfStarvingDays() const;
	int getStarved() const {return _starved;}
	
	void incrementMoveHomeActionsExecuted(){_moveHomeActionsExecuted++;}
	void incrementForageActionsExecuted(){_forageActionsExecuted++;}

	
	
	int reductionResourcesToCategory(long r) const
	{
		/*
			r = #calories or #resources.
			r/2000 specifies aprox needs per HG per day
		 */
		int c = computeConsumedResources(1);
		
		if(c==0)
		{
			return 4;
		}
		
		r = r/c;
		if (r < 2)
		{
			return 0;
		}
		if (r < 15)
		{
			return 1;
		}
		if (r < 40) 
		{
			return 2;
		}
		if (r < 100) 
		{
			return 3;
		}

		return 4;		
	}	
	
	void setResourcePrediction(float x) { _resourcePrediction = x;}
	float getResourcePrediction() const { return _resourcePrediction;}	
	
	int getLastMoveCrono() { return _lastMoveCrono; }
	void setLastMoveCrono(int s) { _lastMoveCrono = s; }
};

} // namespace Gujarat

#endif // __GujaratAgent_hxx__

