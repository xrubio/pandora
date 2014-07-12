
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

void	OriginalDemographics::checkMortality( GujaratAgent& agent )
{
	int originalPopulation = agent.getPopulationSize();
	int popSize = agent.getPopulationSize();
	std::cout << "percentage of starving: " << agent.getPercentageOfStarvingDays() << std::endl;
	agent.checkDeath(0, 100, agent.getPercentageOfStarvingDays());
	int deathByStarvation = popSize - agent.getPopulationSize();
	popSize = agent.getPopulationSize();

	// Adult mortality check
	agent.checkDeath( 15, 1000, 1.5f );
	agent.checkDeath( 3, 15, 1.5f );
	int adultsNaturalDeath = popSize - agent.getPopulationSize();
	popSize = agent.getPopulationSize();
	// Children mortality check
	agent.checkDeath( 0, 3, 10 );
	int childNaturalDeath = popSize - agent.getPopulationSize();
	popSize = agent.getPopulationSize();

	std::cout << "agent: " << agent << " starved: " << agent.getStarved() << " days - percentage: " << agent.getPercentageOfStarvingDays() << " original pop: " << originalPopulation << " current pop: " << popSize << " death by starvation: " << deathByStarvation << " adults natural death: " << adultsNaturalDeath << " children natural death: " << childNaturalDeath << std::endl;
}

void	OriginalDemographics::checkReproduction( GujaratAgent& agent )
{
	if (!agent.canReproduce()) return;
	
	// regular behavior is 1 child every 2 years
	if(Engine::GeneralState::statistics().getUniformDistValue(0,1)==0)
	{
		agent.addNewChild();
	}
}

}

