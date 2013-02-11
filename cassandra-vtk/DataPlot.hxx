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

#ifndef __DataPlot_hxx__
#define __DataPlot_hxx__

#include <qwt_plot.h>

class QwtPlotCurve;
class QwtPlotZoomer;

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{
// TODO convert data plot children to template
// this basic plotter shows a given value for the entire simulation (x = step)
class DataPlot : public QwtPlot
{
	Q_OBJECT
	QwtPlotCurve * _valuePlot;
	QwtPlotZoomer * _zoomer;

	void updateMinMaxValue();
protected:
	Engine::SimulationRecord * _simulationRecord;
//	QwtArray<double> _steps;
//	QwtArray<double> _values;
	QVector<double> _steps;
	QVector<double> _values;

	double _minValue;
	double _maxValue;
public:
	DataPlot(QWidget * parent = 0);
	virtual ~DataPlot();
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
	virtual void calculateValues( const std::string & type, const std::string & state ) = 0;
	void stateSelected( const std::string & type, const std::string & state );

protected slots:
	void mouseMoved( const QPoint & point );
};

} // namespace GUI

#endif // __DataPlot_hxx__
