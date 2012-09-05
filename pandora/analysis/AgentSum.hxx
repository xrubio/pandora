
#ifndef __AgentSum_hxx__
#define __AgentSum_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class AgentSum : public AgentAnalysis
{
	std::string _attributeName;
public:
	AgentSum( const std::string & attributeName );
	virtual ~AgentSum();
	void computeAgent( const Engine::AgentRecord & agentRecord );
};

} // namespace Analysis

#endif // __AgentSum_hxx__

