/*
 * Copyright (c) 2012
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

#include <AgentTraitSelection.hxx>
#include <SimulationRecord.hxx>
#include <AgentRecord.hxx>

#include <algorithm>

#include <iostream>
//#include <QLabel>
//#include <QListWidget>

namespace GUI
{

AgentTraitSelection::AgentTraitSelection(QWidget * parent ) : QListWidget(parent), _simulationRecord(0), _selectedType("unknown")
{	  
	setMouseTracking(true);
}

AgentTraitSelection::~AgentTraitSelection()
{
}

void AgentTraitSelection::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_simulationRecord = simulationRecord;

	_selectedType = "unknown";
	_states.clear();
	clear();
	update();
}
	
void AgentTraitSelection::typeSelected( QListWidgetItem * item )
{
	_selectedType= item->text().toStdString();
	_states.clear();
	clear();
	for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it=_simulationRecord->beginAgents(_selectedType); it!=_simulationRecord->endAgents(_selectedType); it++)
	{
		Engine::AgentRecord * agentRecord = it->second;
        // int
    	for(Engine::AgentRecord::IntAttributesMap::const_iterator itS=agentRecord->beginInt(); itS!=agentRecord->endInt(); itS++)
	    {
		   	std::string stateName = itS->first;
		    std::list<std::string>::const_iterator itL = std::find(_states.begin(), _states.end(), stateName);
    		if(itL==_states.end())
	    	{
		   		_states.push_back(stateName);
		    	addItem(stateName.c_str());
    		}
	    }
        // float
        for(Engine::AgentRecord::FloatAttributesMap::const_iterator itS=agentRecord->beginFloat(); itS!=agentRecord->endFloat(); itS++)
	    {
		   	std::string stateName = itS->first;
		    std::list<std::string>::const_iterator itL = std::find(_states.begin(), _states.end(), stateName);
    		if(itL==_states.end())
	    	{
		   		_states.push_back(stateName);
		    	addItem(stateName.c_str());
    		}
	    }   
        // strings
        for(Engine::AgentRecord::StrAttributesMap::const_iterator itS=agentRecord->beginStr(); itS!=agentRecord->endStr(); itS++)
	    {
		   	std::string stateName = itS->first;
		    std::list<std::string>::const_iterator itL = std::find(_states.begin(), _states.end(), stateName);
    		if(itL==_states.end())
	    	{
		   		_states.push_back(stateName);
		    	addItem(stateName.c_str());
    		}
	    }
	}    
}

} // namespace GUI

