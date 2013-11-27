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

#include <RasterAnalysisSelection.hxx>
#include <SimulationRecord.hxx>

namespace GUI
{

RasterAnalysisSelection::RasterAnalysisSelection( QWidget * parent ) : QWidget(parent)
{
	_selection.setupUi(this);

	_selection.analysis->addItem("Mean", RasterAnalysisSelection::eMean);
	_selection.analysis->addItem("Sum", RasterAnalysisSelection::eSum);
//	_selection.analysis->addItem("Standard Deviation", RasterAnalysisSelection::eStandardDeviation);

	connect(_selection.analysis, SIGNAL(activated(const QString &)), this, SLOT(analysisSelected(const QString &)));
	connect(_selection.removeButton, SIGNAL(clicked()), this, SLOT(removeAnalysis()));
}
	
RasterAnalysisSelection::~RasterAnalysisSelection()
{
}

void RasterAnalysisSelection::removeAnalysis()
{
	emit removeAnalysis(this);
}

void RasterAnalysisSelection::analysisSelected( const QString & )
{
	// TODO change widget depending on option
	/*
	if(option.isEmpty())
	{
		return;
	}
	_selection.traits->setEnabled(true);
	*/
}

RasterAnalysisSelection::GlobalAnalysis RasterAnalysisSelection::getAnalysis() const
{
	return (GlobalAnalysis)_selection.analysis->findData(_selection.analysis->currentIndex());
}

} // namespace GUI

