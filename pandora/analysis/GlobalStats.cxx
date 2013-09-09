/*
 * Copyright (c) 2013
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <analysis/GlobalStats.hxx>
#include <analysis/Analysis.hxx>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace PostProcess
{

GlobalStats::GlobalStats( const std::string & separator ) : Output(separator), _analysisOwnership(true), _params(0)
{
}

GlobalStats::~GlobalStats()
{
	if(!_analysisOwnership)
	{
		return;
	}

	AgentAnalysisList::iterator it =_analysisList.begin();
	while(it!=_analysisList.end())
	{
		Analysis * analysis = *it;
		it = _analysisList.erase(it);
		delete analysis;
	}

	if(_params)
	{
		delete _params;
	}
}

void GlobalStats::setAnalysisOwnership( bool analysisOwnership )
{
	_analysisOwnership = analysisOwnership;
}

void GlobalStats::apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type )
{	
	std::cout << "Executing postprocess: " << getName() << " ...";
	// pre process
	for(AgentAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
	{
		std::cout << "Preprocessing analysis: " << (*itL)->getName() << "...";
		(*itL)->setNumTimeSteps(1+(simRecord.getNumSteps()/simRecord.getFinalResolution()));	
		(*itL)->preProcess();
		std::cout << " done" << std::endl;
	}
	
	std::ofstream file;
	file.open(outputFile.c_str());
  
	std::stringstream header;
	header << "timeStep" << _separator;
	for(AgentAnalysisList::const_iterator it=_analysisList.begin(); it!=_analysisList.end(); it++)
	{
		if((*it)->writeResults())
		{
			header << (*it)->getName() << _separator;
		}
	}
	file << header.str() << std::endl;;

	// all agents
	if(type.compare("all")==0)
	{
		for(Engine::SimulationRecord::AgentTypesMap::const_iterator it=simRecord.beginTypes(); it!=simRecord.endTypes(); it++)
		{
			const Engine::SimulationRecord::AgentRecordsMap & agentRecords = it->second;
			for(AgentAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
			{
				std::cout << "Computing analysis: " << (*itL)->getName() << "...";
				for(Engine::SimulationRecord::AgentRecordsMap::const_iterator itA=agentRecords.begin(); itA!=agentRecords.end(); itA++)
				{
					AgentAnalysis * analysis = (*itL);
					analysis->computeAgent(*(itA->second));
				}
				std::cout << " done" << std::endl;
			}
		}
	}
	else
	{
		if(!simRecord.hasAgentType(type))
		{
			return;
		}
		for(AgentAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
		{
			std::cout << "Computing analysis: " << (*itL)->getName() << "...";
			for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it=simRecord.beginAgents(type); it!=simRecord.endAgents(type); it++)
			{
				AgentAnalysis * analysis = (AgentAnalysis*)(*itL);
				analysis->computeAgent(*(it->second));
			}
			std::cout << " done" << std::endl;
		}
	}	
	
	// post process
	for(AgentAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
	{
		std::cout << "Postprocessing analysis: " << (*itL)->getName() << "...";
		(*itL)->postProcess();
		std::cout << " done" << std::endl;
	}

	for(int i=0; i<=simRecord.getNumSteps(); i=i+simRecord.getFinalResolution())
	{
		std::stringstream newLine;
		newLine << i << _separator;
		for(AgentAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
		{
			if((*itL)->writeResults())
			{
				newLine << std::setprecision(2) << std::fixed << (*itL)->getResult(i/simRecord.getFinalResolution()) << _separator;				
			}
		}
		file << newLine.str() << std::endl;
	}
	file.close();
	std::cout << "done!" << std::endl;
	if(_params)
	{
		std::ofstream groupFile;
		std::cout << "grouping by params" << std::endl;
		groupFile.open(_groupFile.c_str(), std::ios_base::app);
		std::stringstream line;
		unsigned pos = outputFile.find_last_of("/");
		std::string fileName = outputFile.substr(pos+1);
		line << fileName << _separator;
		for(AgentAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
		{
			line << std::setprecision(2) << std::fixed << (*itL)->getResult(simRecord.getNumSteps()/simRecord.getFinalResolution()) << _separator;				
		}
		groupFile << line.str() << std::endl;
		groupFile.close();
	}
}
void GlobalStats::addAnalysis( AgentAnalysis * analysis )
{
	_analysisList.push_back(analysis);
}

void GlobalStats::setParams( Params * params, const std::string & groupFile )
{
	_params = params;
	_groupFile = groupFile;
	
	std::ofstream file;
	file.open(_groupFile.c_str());
  
	std::stringstream header;
	header << "run" << _separator;
	for(AgentAnalysisList::const_iterator it=_analysisList.begin(); it!=_analysisList.end(); it++)
	{
		if((*it)->writeResults())
		{
			header << (*it)->getName() << _separator;
		}
	}
	// TODO params
	file << header.str() << std::endl;;
	file.close();
}

std::string GlobalStats::getName() const
{
	return "Global Stats";
}

} // namespace PostProcess

