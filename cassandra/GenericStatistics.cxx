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

#include <GenericStatistics.hxx>
#include <MeanDataPlot.hxx>
#include <SumDataPlot.hxx>
#include <StepDataPlot.hxx>

#include <QListWidgetItem>
#include <QHBoxLayout>

#include <iostream>

namespace GUI
{

GenericStatistics::GenericStatistics( QWidget * parent ) : QWidget(parent), _meanDataPlot(0), _sumDataPlot(0), _stepDataPlot(0)
{
	QHBoxLayout * layout= new QHBoxLayout;

	_stepDataPlot = new StepDataPlot(this);
	_stepDataPlot->setTitle("Samples");
	layout->addWidget(_stepDataPlot);

	_meanDataPlot = new MeanDataPlot(this);
	_meanDataPlot->setTitle("Mean");
	layout->addWidget(_meanDataPlot);

	_sumDataPlot = new SumDataPlot(this);
	_sumDataPlot->setTitle("Sum");
	layout->addWidget(_sumDataPlot);
	
	setLayout(layout);
}

GenericStatistics::~GenericStatistics()
{
}

void GenericStatistics::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_meanDataPlot->setSimulationRecord(simulationRecord);
	_sumDataPlot->setSimulationRecord(simulationRecord);
	_stepDataPlot->setSimulationRecord(simulationRecord);
}

void GenericStatistics::calculateStatistics( const std::string & type, const std::string & state )
{
	std::cout << "stats for type: " << type << " and state: " << state << std::endl;
	_meanDataPlot->stateSelected(type, state);
	_sumDataPlot->stateSelected(type, state);
	_stepDataPlot->stateSelected(type, state);
}

void GenericStatistics::viewedStepChangedSlot( int newViewedStep )
{
	_stepDataPlot->stepChanged(newViewedStep);
}

} // namespace GUI

