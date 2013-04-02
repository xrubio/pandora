#ifndef __QuantumWorldConfig_hxx__
#define __QuantumWorldConfig_hxx__

#include <Config.hxx>

namespace QuantumExperiment 
{

class QuantumWorldConfig : public Engine::Config
{
	int _size;

	// agents
	int _numAgents;
	int _neededResources;

	//mdp
	int _horizon;
	int _width;
	int _explorationBonus;

public:
	QuantumWorldConfig();
	virtual ~QuantumWorldConfig();
	void extractParticularAttribs(TiXmlElement *pRoot);
	int getSize() const;
	
	friend class QuantumWorld;
	friend class Forager;
};

} // namespace QuantumExperiment

#endif 
