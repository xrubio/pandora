#include <QuantumWorldConfig.hxx>

namespace QuantumExperiment
{

QuantumWorldConfig::QuantumWorldConfig( const std::string & fileName ) : Config(fileName)
{
}

QuantumWorldConfig::~QuantumWorldConfig()
{
}

void QuantumWorldConfig::loadParams()
{
	// agents
    _numAgents = getParamInt("agents", "num");
    _neededResources = getParamInt("agents", "neededResources");
    _perfectInformation = getParamBool("agents", "perfectInformation");
    _riskAversion = getParamFloat("agents", "riskAversion");

	//mdp
    _horizon = getParamInt("agents/mdp", "horizon");
    _width = getParamInt("agents/mdp", "width");
    _explorationBonus = getParamInt("agents/mdp", "explorationBonus");
}

} // namespace QuantumExperiment

