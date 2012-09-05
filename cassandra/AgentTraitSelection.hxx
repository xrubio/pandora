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

#ifndef __AgentTraitSelection_hxx__
#define __AgentTraitSelection_hxx__

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

class AgentTraitSelection : public QListWidget
{
	Q_OBJECT

	Engine::SimulationRecord * _simulationRecord;
	std::list<std::string> _states;
	std::string _selectedType;
public:

	AgentTraitSelection(QWidget * parent = 0);
	virtual ~AgentTraitSelection();
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );

public slots:
	void typeSelected( QListWidgetItem * item );
}; 

} // namespace GUI

#endif // __AgentTraitSelection_hxx__

