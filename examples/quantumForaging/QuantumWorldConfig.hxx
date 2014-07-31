#ifndef __QuantumWorldConfig_hxx__
#define __QuantumWorldConfig_hxx__

#include <Config.hxx>
#include <Size.hxx>

namespace QuantumExperiment 
{

class QuantumWorldConfig : public Engine::Config
{
	// agents
	int _numAgents;
	int _neededResources;
	// if true, the agents will know at the beginning of the simulation the real resource map
	bool _perfectInformation;
	// aversion to risk, from 0.0 to 1.0f
	float _riskAversion;

	//mdp
	int _horizon;
	int _width;
	int _explorationBonus;

public:
	QuantumWorldConfig( const std::string & fileName );
	virtual ~QuantumWorldConfig();
	void loadParams(); 
	
	friend class QuantumWorld;
};

} // namespace QuantumExperiment

#endif 
