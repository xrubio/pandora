/*
 * Copyright (c) 2013
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <AnalysisControlThread.hxx>
#include <Analysis.hxx>

#include <iostream>

namespace GUI
{

AnalysisControlThread::AnalysisControlThread( const std::string & baseDir, const std::string & agentType, const std::string & outputDir ) : _baseDir(baseDir), _agentType(agentType), _outputDir(outputDir)
{
}

AnalysisControlThread::~AnalysisControlThread()
{
	std::list<Analysis::AgentAnalysis *>::iterator it=_analysisToPerform.begin();
	while(it!=_analysisToPerform.end())
	{
		Analysis::AgentAnalysis * analysis = *it;
		it = _analysisToPerform.erase(it);
		delete analysis;
	}
}

void AnalysisControlThread::run()
{
	_cancelExecution = false;
	for(std::list<Analysis::AgentAnalysis *>::iterator it=_analysisToPerform.begin(); it!=_analysisToPerform.end(); it++)
	{
		std::cout << "performing next analysis" << std::endl;	
		
		if(_cancelExecution)
		{
			quit();
			return;
		}
	}
	emit nextSimulation();
}

void AnalysisControlThread::addAnalysis( Analysis::AgentAnalysis * analysis )
{
	_analysisToPerform.push_back(analysis);
}

void AnalysisControlThread::cancelExecution()
{
	_cancelExecution = true;
}

int AnalysisControlThread::getNumberOfSimulations()
{
	return 10;
}

} // namespace GUI

