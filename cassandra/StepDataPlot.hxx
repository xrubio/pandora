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

#ifndef __StepDataPlot_hxx__
#define __StepDataPlot_hxx__

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
// this basic plotter shows the complete set of values for the current step
class StepDataPlot : public QwtPlot
{
	Q_OBJECT
	QwtPlotCurve * _valuePlot;
	QwtPlotZoomer * _zoomer;

protected:
	Engine::SimulationRecord * _simulationRecord;
	QVector<double> _values;
	QVector<double> _numSamples;
//	QwtArray<double> _values;
//	QwtArray<double> _numSamples;

	float _minValue;
	float _maxValue;

	std::string _type;
	std::string _state;
	int _step;
	void updateData();
	
	//void fillHistogram( const std::string & type, const std::string & state, int step, QwtArray<double> & numSamples, const int & minValue );
	void fillHistogram( const std::string & type, const std::string & state, int step, QVector<double> & numSamples, const int & minValue );
	void fillStepMinMaxValues( const std::string & type, const std::string & state, int step, float & minValue, float & maxValue );
public:
	StepDataPlot(QWidget * parent = 0);
	virtual ~StepDataPlot();
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
	void stateSelected( const std::string & type, const std::string & state );
	void stepChanged(int step );
};

} // namespace GUI

#endif // __StepDataPlot_hxx__

