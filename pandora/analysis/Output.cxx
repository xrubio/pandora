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


#include <analysis/Output.hxx>

#include <analysis/Analysis.hxx>
#include <SimulationRecord.hxx>

#include <iostream>
#include <fstream>
#include <iomanip>

namespace PostProcess
{

Output::Output( const std::string & separator ) : _separator(separator)
{
}

Output::~Output()
{
}



/*
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
*/

void Output::preProcess(  const Engine::SimulationRecord & simRecord, const std::string & outputFile )
{
}

void Output::computeAgent( const Engine::AgentRecord & agentRecord )
{
}

void Output::postProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile )
{
}

void Output::apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type )
{
	std::cout << "Executing postprocess: " << getName() << " ...";
	preProcess(simRecord, outputFile);
	// all agents
	if(type.compare("all")==0)
	{
		for(Engine::SimulationRecord::AgentTypesMap::const_iterator it=simRecord.beginTypes(); it!=simRecord.endTypes(); it++)
		{		
			const Engine::SimulationRecord::AgentRecordsMap & agentRecords = it->second;
			for(Engine::SimulationRecord::AgentRecordsMap::const_iterator itA=agentRecords.begin(); itA!=agentRecords.end(); itA++)
			{
				computeAgent(*(itA->second));
			}
		}
	}
	else
	{
		if(!simRecord.hasAgentType(type))
		{
			return;
		}	
		
		for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it=simRecord.beginAgents(type); it!=simRecord.endAgents(type); it++)
		{
			computeAgent(*(it->second));
		}
	}	
	std::cout << " done" << std::endl;
	postProcess(simRecord, outputFile);
}

} // namespace PostProcess

