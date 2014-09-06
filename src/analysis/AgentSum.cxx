
#include <analysis/AgentSum.hxx>
#include <AgentRecord.hxx>

namespace PostProcess
{

AgentSum::AgentSum( const std::string & attributeName ) : AgentAnalysis("Sum "+attributeName), _attributeName(attributeName)
{
}

AgentSum::~AgentSum()
{
}

void AgentSum::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(unsigned i=0; i<_results.size(); i++)
	{        
        double value = 0.0f;
        if(agentRecord.isInt(_attributeName))
        {
            value = agentRecord.getInt(i, _attributeName);
        }
        else if(agentRecord.isFloat(_attributeName))
        {  
            value = agentRecord.getFloat(i, _attributeName);
        }
		_results[i] += value;
	}
}

} // namespace PostProcess

