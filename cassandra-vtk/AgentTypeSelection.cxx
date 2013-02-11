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

#include <AgentTypeSelection.hxx>
#include <SimulationRecord.hxx>

#include <algorithm>

#include <iostream>

namespace GUI
{

AgentTypeSelection::AgentTypeSelection(QWidget * parent ) : QListWidget(parent), _simulationRecord(0)
{	  
	setMouseTracking(true);
}

AgentTypeSelection::~AgentTypeSelection()
{
}

void AgentTypeSelection::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_simulationRecord = simulationRecord;
	clear();
	if(!simulationRecord)
	{
		update();
		return;
	}
	for(Engine::SimulationRecord::AgentTypesMap::const_iterator it=_simulationRecord->beginTypes(); it!=_simulationRecord->endTypes(); it++)
	{
		addItem(it->first.c_str());
	}    
	update();
}

} // namespace GUI

