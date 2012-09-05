
#include <analysis/Results.hxx>

#include <analysis/Analysis.hxx>
#include <SimulationRecord.hxx>

#include <iostream>
#include <fstream>

namespace Analysis
{

Results::Results( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type, const std::string & separator ) : _simRecord(simRecord), _outputFile(outputFile), _type(type), _separator(separator)
{
}

Results::~Results()
{
	AnalysisList::iterator it =_analysisList.begin();
	while(it!=_analysisList.end())
	{
		Analysis * analysis = *it;
		it = _analysisList.erase(it);
		delete analysis;
	}
}

void Results::apply() const
{	
	// pre process
	for(AnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
	{
		std::cout << "Preprocessing analysis: " << (*itL)->getName() << "...";
		(*itL)->preProcess();
		std::cout << " done" << std::endl;
	}
	
	std::ofstream file;
	file.open(_outputFile.c_str());
  
	std::stringstream header;
	header << "timeStep" << _separator;
	for(AnalysisList::const_iterator it=_analysisList.begin(); it!=_analysisList.end(); it++)
	{
		if((*it)->writeResults())
		{
			header << (*it)->getName() << _separator;
		}
	}
	file << header.str() << std::endl;;

	concreteApply();
	
	// post process
	for(AnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
	{
		std::cout << "Postprocessing analysis: " << (*itL)->getName() << "...";
		(*itL)->postProcess();
		std::cout << " done" << std::endl;
	}

	for(int i=0; i<=_simRecord.getNumSteps()/_simRecord.getResolution(); i++)		
	{
		std::stringstream newLine;
		newLine << i << _separator;
		for(AnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
		{
			if((*itL)->writeResults())
			{
				newLine << (*itL)->getResult(i) << _separator;				
			}
		}
		file << newLine.str() << std::endl;
	}
	file.close();
}

void Results::addAnalysis( Analysis * analysis )
{
	analysis->setNumTimeSteps(1+(_simRecord.getNumSteps()/_simRecord.getResolution()));	
	_analysisList.push_back(analysis);
}

	
AgentResults::AgentResults( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type, const std::string & separator ) : Results(simRecord, outputFile, type, separator)
{
}

void AgentResults::concreteApply() const
{
	// all agents
	if(_type.compare("all")==0)
	{
		for(Engine::SimulationRecord::AgentTypesMap::const_iterator it=_simRecord.beginTypes(); it!=_simRecord.endTypes(); it++)
		{
			const Engine::SimulationRecord::AgentRecordsMap & agentRecords = it->second;
			for(AnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
			{
				std::cout << "Computing analysis: " << (*itL)->getName() << "...";
				for(Engine::SimulationRecord::AgentRecordsMap::const_iterator itA=agentRecords.begin(); itA!=agentRecords.end(); itA++)
				{
					AgentAnalysis * analysis = (AgentAnalysis*)(*itL);
					analysis->computeAgent(*(itA->second));
				}
				std::cout << " done" << std::endl;
			}
		}
	}
	else
	{
		if(!_simRecord.hasAgentType(_type))
		{
			return;
		}
		for(AnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
		{
			std::cout << "Computing analysis: " << (*itL)->getName() << "...";
			for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it=_simRecord.beginAgents(_type); it!=_simRecord.endAgents(_type); it++)
			{
				AgentAnalysis * analysis = (AgentAnalysis*)(*itL);
				analysis->computeAgent(*(it->second));
			}
			std::cout << " done" << std::endl;
		}
	}	
}

RasterResults::RasterResults( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type, const std::string & separator ) : Results(simRecord, outputFile, type, separator)
{
}

void RasterResults::concreteApply() const
{
	// TODO what about static rasters?
	const Engine::SimulationRecord::RasterHistory & rasterHistory = _simRecord.getRasterHistory(_type);
	for(AnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
	{
		std::cout << "Computing analysis: " << (*itL)->getName() << "...";
		RasterAnalysis * analysis = (RasterAnalysis*)(*itL);
		analysis->computeRaster(rasterHistory);
		std::cout << " done" << std::endl;
	}
}

} // namespace Analysis

