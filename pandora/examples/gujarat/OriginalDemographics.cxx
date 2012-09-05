
#include <OriginalDemographics.hxx>

#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <GujaratAgent.hxx>
#include <GujaratWorld.hxx>
#include <GujaratConfig.hxx>

namespace Gujarat
{

OriginalDemographics::OriginalDemographics()
{
}

OriginalDemographics::~OriginalDemographics()
{
}

bool	OriginalDemographics::checkEmigration(  GujaratAgent& agent )
{
	return false;
}

void	OriginalDemographics::checkMortality( GujaratAgent& agent )
{

	/*
	// TODO check resources, now year, must be seasonal
	int popSize = agent.getPopulationSize();
	// each individual eats 10 resources
	int maintainedPopulation = agent.getOnHandResources()/2000.0f;
	int starvingPopulation = popSize - maintainedPopulation;
	// for each starving pop, possibility of death = 10% for each individual
	if(starvingPopulation>0)
	{
		agent.decimatePopulation();
	}
	*/

	int originalPopulation = agent.getPopulationSize();
	int popSize = agent.getPopulationSize();
	agent.checkDeath(0, 1000, agent.getPercentageOfStarvingDays());
	int deathByStarvation = popSize - agent.getPopulationSize();
	popSize = agent.getPopulationSize();

	// Adult mortality check
	agent.checkDeath( 15, 1000, 2 );
	agent.checkDeath( 3, 15, 2 );
	int adultsNaturalDeath = popSize - agent.getPopulationSize();
	popSize = agent.getPopulationSize();
	// Children mortality check
	agent.checkDeath( 0, 3, 10 );
	int childNaturalDeath = popSize - agent.getPopulationSize();
	popSize = agent.getPopulationSize();

	std::cout << "agent: " << agent << " starved: " << agent.getStarved() << " days - percentage: " << agent.getPercentageOfStarvingDays() << " original pop: " << originalPopulation << " current pop: " << popSize << " death by starvation: " << deathByStarvation << " adults natural death: " << adultsNaturalDeath << " children natural death: " << childNaturalDeath << std::endl;
	
	// Old age death check
	//agent.checkDeathByAging( 50 );
}

void	OriginalDemographics::checkReproduction( GujaratAgent& agent )
{
	if (!agent.canReproduce()) return;
	
	// regular behavior is 1 child every 2 years
	if(Engine::GeneralState::statistics().getUniformDistValue(0,1)==0)
	{
		agent.addNewChild();
	}
	//agent.addNewChild();
}

}

