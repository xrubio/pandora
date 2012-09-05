
#include <analysis/AgentMean.hxx>
#include <AgentRecord.hxx>

namespace Analysis
{

AgentMean::AgentMean( const std::string & attributeName ) : AgentAnalysis("Mean "+attributeName), _attributeName(attributeName)
{
}

AgentMean::~AgentMean()
{
}

void AgentMean::preProcess()
{
	_numAgents.resize(_results.size());
}

void AgentMean::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(int i=0; i<_results.size(); i++)
	{
		if(agentRecord.getState(i, "exists"))
		{
			int value = agentRecord.getState(i, _attributeName);
			_results[i] += value;
			_numAgents[i]++;
		}
	}
}

void AgentMean::postProcess()
{
	for(int i=0; i<_results.size(); i++)
	{
		int numAgents = _numAgents.at(i);
		if(numAgents>0)
		{
			_results[i] /= numAgents;
		}
		else
		{
			_results[i] = 0.0f;
		}
	}
}

} // namespace Analysis

