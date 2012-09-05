
#include <analysis/Analysis.hxx>
#include <AgentRecord.hxx>

namespace Analysis
{

Analysis::Analysis( const std::string & name, bool writeResults ) : _name(name), _writeResults(writeResults)
{
}

Analysis::~Analysis()
{
}

void Analysis::setNumTimeSteps( int numTimeSteps )
{
	_results.clear();
	_results.resize(numTimeSteps);
}

const std::string & Analysis::getName() const
{
	return _name;
}
	
float Analysis::getResult( int timeStep ) const
{
	return _results.at(timeStep);
}
	
} // namespace Analysis

