
#include <analysis/AgentSum.hxx>
#include <AgentRecord.hxx>

namespace Analysis
{

AgentSum::AgentSum( const std::string & attributeName ) : AgentAnalysis("Sum "+attributeName), _attributeName(attributeName)
{
}

AgentSum::~AgentSum()
{
}

void AgentSum::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(int i=0; i<_results.size(); i++)
	{
		int value = agentRecord.getState(i, _attributeName);
		_results[i] += value;
	}
}

} // namespace Analysis

