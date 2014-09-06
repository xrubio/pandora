
#include <analysis/AgentNum.hxx>
#include <AgentRecord.hxx>

namespace PostProcess
{

AgentNum::AgentNum() : AgentAnalysis("Number of agents")
{
}

AgentNum::~AgentNum()
{
}

void AgentNum::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(unsigned i=0; i<_results.size(); i++)
	{
		int value = agentRecord.getInt(i, "exists");
		_results[i] += value;
	}
}

} // namespace PostProcess

