
#include <RamirezDemographics.hxx>

#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <GujaratAgent.hxx>

namespace Gujarat
{

RamirezDemographics::RamirezDemographics()
{
}

RamirezDemographics::~RamirezDemographics()
{
}

bool	RamirezDemographics::checkEmigration(GujaratAgent & agent  )
{
	float breakPoint = agent.getEmigrationProbability() * 10000.0f;
	int die = Engine::GeneralState::statistics().getUniformDistValue(0,10000);
	agent.setEmigrationProbability( 0.0f );
	if ( die < breakPoint ) 
		return true;
	return false;
}

void	RamirezDemographics::checkMortality( GujaratAgent & agent )
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

void	RamirezDemographics::checkReproduction( GujaratAgent & agent )
{
	if (!agent.canReproduce()) return;

	float breakPoint = agent.getReproductionProbability() * 10000.0f;
	int die = Engine::GeneralState::statistics().getUniformDistValue(0,10000);

	agent.setReproductionProbability(0.0f);

	if ( die < breakPoint )
		agent.addNewChild();
}

}
