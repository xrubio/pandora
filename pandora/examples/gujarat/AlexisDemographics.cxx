
#include <AlexisDemographics.hxx>

#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <GujaratAgent.hxx>

namespace Gujarat
{

AlexisDemographics::AlexisDemographics( )	
{
}

AlexisDemographics::~AlexisDemographics()
{
}


/*
bool	AlexisDemographics::checkEmigration( GujaratAgent& agent )
{
	float breakPoint = agent.getEmigrationProbability() * 10000.0f;	
	int die = Engine::GeneralState::statistics().getUniformDistValue(0,10000);
	agent.setEmigrationProbability( 0.0f );
	if ( die < breakPoint ) 
		return true;
	return false;
}
*/

/*
void AlexisDemographics::checkStarvationMortality( GujaratAgent& agent)
{
	int needs = agent.computeConsumedResources(1);
	//log() << "CHECK/starvating> Agent_" << agent._id << "needs:" << needs << "resources:" << agent._collectedResources << std::endl;
	if (agent.getOnHandResources() < needs)
	{
		int deficitRatioPer1000 = (int)((1000.0f * (float)agent.getOnHandResources()) / (float)needs);
		for(unsigned int index=2; index < agent.getPopulationSize(); index++)
		{
			agent.checkIndividualStarvationDeath(index,deficitRatioPer1000);			
		}
	}	
}
*/

void	AlexisDemographics::checkMortality( GujaratAgent& agent )
{
	// TODO:
	// Add attributes:
	// 	* Natural Adult Mortality (in 1000%)
	//	* Natural Children Mortality (in 1000%)
	//	* Elderly Age Threshold
	// Adult mortality check
	agent.checkDeath( 15, 1000, 2 );
	// Children mortality check
	agent.checkDeath( 0, 3, 10 );
	agent.checkDeath( 3, 15, 2 );
	// Old age death check
	agent.checkDeathByAging( 50 );
}

void	AlexisDemographics::checkReproduction( GujaratAgent& agent )
{
	if (!agent.canReproduce()) return;

	float breakPoint = agent.getReproductionProbability() * 10000.0f;
	int die = Engine::GeneralState::statistics().getUniformDistValue(0,10000);

	agent.setReproductionProbability(0.0f);

	if ( die < breakPoint )
		agent.addNewChild();
}

}
