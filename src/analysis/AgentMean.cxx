
#include <analysis/AgentMean.hxx>
#include <AgentRecord.hxx>

namespace PostProcess
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
	for(unsigned i=0; i<_numAgents.size(); i++)
	{
		_numAgents.at(i) = 0;
		_results.at(i) = 0.0f;
	}
}

void AgentMean::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(unsigned i=0; i<_results.size(); i++)
	{
		if(agentRecord.getInt(i, "exists"))
		{
            float value = 0.0f;
            if(agentRecord.isInt(_attributeName))
            {
                value = agentRecord.getInt(i, _attributeName);
            }
            else if(agentRecord.isFloat(_attributeName))
            {  
                value = agentRecord.getFloat(i, _attributeName);
            }
			_results[i] += value;
			_numAgents[i]++;
		}
	}
}

void AgentMean::postProcess()
{
	for(unsigned i=0; i<_results.size(); i++)
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

} // namespace PostProcess

