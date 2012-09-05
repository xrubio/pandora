
#ifndef __AgentNum_hxx__
#define __AgentNum_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace Analysis
{

class AgentNum : public AgentAnalysis
{
public:
	AgentNum();
	virtual ~AgentNum();
	void computeAgent( const Engine::AgentRecord & agentRecord );
};

} // namespace Analysis

#endif // __AgentNum_hxx__

