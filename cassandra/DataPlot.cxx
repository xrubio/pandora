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

#include <DataPlot.hxx>

#include <SimulationRecord.hxx>
#include <Exception.hxx>

#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>

#include <iostream>
#include <sstream>
#include <limits>

namespace GUI
{

DataPlot::DataPlot( QWidget * parent ) : QwtPlot(parent), _valuePlot(0), _zoomer(0), _simulationRecord(0), _steps(0,0), _values(0,0), _minValue(0), _maxValue(0)
{
	// Set axis titles
	setAxisTitle(xBottom, "step");
	setAxisTitle(yLeft, "value");
  
	_valuePlot = new QwtPlotCurve();
	_valuePlot->setStyle(QwtPlotCurve::Lines);
	_valuePlot->setPen(QPen(Qt::red));
	_valuePlot->attach(this);	
	
	_zoomer = new QwtPlotZoomer(canvas());   
	_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    _zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

	//_zoomer->setRubberBandPen(QPen(Qt::black, 1, Qt::DotLine));
	//_zoomer->setTrackerPen(QPen(Qt::red));
	_zoomer->setTrackerMode(QwtPlotZoomer::AlwaysOn);
	//_zoomer->setRubberBand(QwtPlotZoomer::VLineRubberBand);

	QwtPlotPanner *panner = new QwtPlotPanner(canvas());
	panner->setMouseButton(Qt::MidButton);

	/*
	QwtPlotPicker * d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPicker::NoSelection, 
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, 
        canvas());
    d_picker->setRubberBandPen(QColor(Qt::green));
    d_picker->setRubberBand(QwtPicker::CrossRubberBand);
    d_picker->setTrackerPen(QColor(Qt::white));

	connect(d_picker, SIGNAL(moved(const QPoint &)), SLOT(mouseMoved(const QPoint &)));
	*/
	replot();
}

DataPlot::~DataPlot()
{
}

void DataPlot::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_simulationRecord = simulationRecord;

	if(!_simulationRecord)
	{
		return;
	}

	int realSize = _simulationRecord->getNumSteps()/_simulationRecord->getFinalResolution();
	_steps.resize(realSize);
	// it doesn't count
	realSize++;
	std::cout << "steps: " << realSize << " with sim num steps: " << _simulationRecord->getNumSteps() << std::endl;
	_values.fill(1, realSize);
	for(int i=0; i<_steps.size(); i++)
	{
		_steps[i] = i*_simulationRecord->getFinalResolution();
	}
	//_valuePlot->setRawData(_steps.data(), _values.data(), _steps.size());
	_valuePlot->setRawSamples(_steps.data(), _values.data(), _steps.size());

	replot();
	_zoomer->setZoomBase();
}

void DataPlot::updateMinMaxValue()
{
	_maxValue = 0;
	_minValue = std::numeric_limits<double>::max();
	for(int i=0; i<_values.size(); i++)
	{
		if(_values[i] > _maxValue)
		{
			_maxValue = _values[i];
		}
		if(_values[i] < _minValue)
		{
			_minValue = _values[i];
		}
	}
	if(_minValue==_maxValue)
	{
		_minValue--;
		_maxValue++;
	}
}

void DataPlot::stateSelected( const std::string & type, const std::string & state )
{
	calculateValues(type, state);
	updateMinMaxValue();
	
	setAxisScale(xBottom, 0, _steps.size()*_simulationRecord->getFinalResolution());
    setAxisScale(yLeft, _minValue, _maxValue);	

	replot();
	_zoomer->setZoomBase();
}

void DataPlot::mouseMoved( const QPoint & point )
{
	std::cout << "mouse moved: " << point.x() << "/" << point.y() << std::endl;
}

} // namespace GUI

