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

#ifndef __GenericStatics_hxx__
#define __GenericStatics_hxx__

#include <QWidget>

class QListWidgetItem;

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{
class MeanDataPlot;
class SumDataPlot;
class StepDataPlot;

class GenericStatistics : public QWidget
{
	Q_OBJECT

	MeanDataPlot * _meanDataPlot;
	SumDataPlot * _sumDataPlot;
	StepDataPlot * _stepDataPlot;
public:
	GenericStatistics(QWidget * parent);
	virtual ~GenericStatistics();
	
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
public slots:
	void calculateStatistics( const std::string & type, const std::string & state );
	void viewedStepChangedSlot( int newViewedStep );
};

} // namespace GUI

#endif // __GenericStatics_hxx__

