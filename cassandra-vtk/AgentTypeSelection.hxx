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

#ifndef __AgentTypeSelection_hxx__
#define __AgentTypeSelection_hxx__

//#include <QWidget>
#include <list>
#include <string>

#include <QListWidget>

class QListWidget;

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{

class AgentTypeSelection : public QListWidget
{
	Q_OBJECT

	Engine::SimulationRecord * _simulationRecord;
	std::list<std::string> _types;
public:

	AgentTypeSelection(QWidget * parent = 0);
	virtual ~AgentTypeSelection();
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
}; 

} // namespace GUI

#endif // __AgentTypeSelection_hxx__

