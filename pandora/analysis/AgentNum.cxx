
#include <analysis/AgentNum.hxx>
#include <AgentRecord.hxx>

namespace Analysis
{

AgentNum::AgentNum() : AgentAnalysis("Number of agents")
{
}

AgentNum::~AgentNum()
{
}

void AgentNum::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(int i=0; i<_results.size(); i++)
	{
		int value = agentRecord.getState(i, "exists");
		_results[i] += value;
	}
}

} // namespace Analysis

