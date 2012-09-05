
#ifndef __AgentMean_hxx__
#define __AgentMean_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class AgentMean : public AgentAnalysis
{
	std::string _attributeName;
	std::vector<int> _numAgents;
public:
	AgentMean( const std::string & attributeName );
	virtual ~AgentMean();
	void computeAgent( const Engine::AgentRecord & agentRecord );

	void preProcess();
	void postProcess();
};

} // namespace Analysis

#endif // __AgentMean_hxx__

