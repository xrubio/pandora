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

#ifndef __SimulationControlThread_hxx__
#define __SimulationControlThread_hxx__

#include <QThread>

namespace GUI
{

class SimulationControlThread : public QThread
{

	Q_OBJECT

	bool _cancelExecution;
	std::string _simulationBinary;
	std::string _outputDir;
	int _totalExperiments;
public:
	SimulationControlThread( const std::string & simulationBinary, const std::string & outputDir, int totalExperiments );
	virtual ~SimulationControlThread();

	void run();

signals:
	void nextSimulation();

public slots:
	void cancelExecution();
};

} // namespace GUI

#endif // __SimulationControlThread_hxx__

