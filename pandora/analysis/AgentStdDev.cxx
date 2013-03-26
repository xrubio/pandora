
#include <analysis/AgentStdDev.hxx>
#include <AgentRecord.hxx>

namespace Analysis
{

AgentStdDev::AgentStdDev( const std::string & attributeName ) : AgentAnalysis("StdDev "+attributeName), _attributeName(attributeName)
{
}

AgentStdDev::~AgentStdDev()
{
}

void AgentStdDev::preProcess()
{
	_numAgents.resize(_results.size());
	_values.resize(_results.size());
}

void AgentStdDev::computeAgent( const Engine::AgentRecord & agentRecord )
{
	for(int i=0; i<_results.size(); i++)
	{
		if(agentRecord.getState(i, "exists"))
		{
			int value = agentRecord.getState(i, _attributeName);
			_values[i].push_back(value);
			_numAgents[i]++;
		}
	}
}

void AgentStdDev::postProcess()
{
	std::vector<float> averages;
	averages.resize(_results.size());
	// mean calculation
	for(int i=0; i<_results.size(); i++)
	{
		int sum = 0;
		for(int j=0; j<_values[i].size(); j++)
		{
			sum += _values[i][j];
		}
		averages[i] = sum/_values[i].size();
	}
	// diff calculations
	for(int i=0; i<_results.size(); i++)
	{
		float diff = 0.0f;
		float average = averages.at(i);
		for(int j=0; j<_values[i].size(); j++)
		{
			float value = _values[i][j];
			diff += (average-value)*(average-value);
		}
		_results[i] = sqrt(diff/_values[i].size());
	}
}

} // namespace Analysis

