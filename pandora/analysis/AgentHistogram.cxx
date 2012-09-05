
#include <analysis/AgentHistogram.hxx>
#include <AgentRecord.hxx>
#include <iostream>
#include <fstream>

namespace Analysis
{

AgentHistogram::AgentHistogram( const std::string & fileName, const std::string & attribute, int interval, int numStep, const std::string & separator ) : AgentAnalysis("Agent Histogram", false), _fileName(fileName), _attribute(attribute), _interval(interval), _numStep(numStep), _separator(separator)
{
}

AgentHistogram::~AgentHistogram()
{
}

void AgentHistogram::preProcess()
{
	// if no num step specified we will pick the last one
	if(_numStep==-1)
	{		
		_numStep = _results.size()-1;
	}
}

void AgentHistogram::computeAgent( const Engine::AgentRecord & agentRecord )
{
	int value = agentRecord.getState(_numStep, _attribute);
	int index = value/_interval;
	if(_histogram.size()<=index)
	{
		_histogram.resize(index+1);
		_histogram.at(index) = 1;
	}
	else
	{
		_histogram.at(index)++;
	}
}

void AgentHistogram::postProcess()
{
	std::ofstream file;
	file.open(_fileName.c_str());
	std::stringstream header;
	header << "beginInterval" << _separator << "time step: " << _numStep << _separator;
	file << header.str() << std::endl;
	for(int i=0; i<_histogram.size(); i++)
	{	
		std::stringstream newLine;
		newLine << i*_interval << _separator << _histogram.at(i) << _separator;
		file << newLine.str() << std::endl;
	}
	file.close();
}

} // namespace Analysis

