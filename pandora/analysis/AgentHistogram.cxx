
#include <analysis/AgentHistogram.hxx>
#include <AgentRecord.hxx>
#include <SimulationRecord.hxx>
#include <iostream>
#include <fstream>

namespace PostProcess 
{

AgentHistogram::AgentHistogram( const std::string & attribute, int interval, int numStep, const std::string & separator) : Output(separator ), _attribute(attribute), _interval(interval), _numStep(numStep)
{
}

AgentHistogram::~AgentHistogram()
{
}

void AgentHistogram::preProcess( const Engine::SimulationRecord & simRecord, const std::string & )
{
	// if no num step specified we will pick the last one
	if(_numStep==-1)
	{		
		_numStep = (simRecord.getNumSteps()/simRecord.getFinalResolution())-1;
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

void AgentHistogram::postProcess( const Engine::SimulationRecord & , const std::string & outputFile )
{
	std::ofstream file;
	file.open(outputFile.c_str());
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

std::string AgentHistogram::getName() const
{
	return "Agent Histogram";
}

} // namespace PostProcess 

