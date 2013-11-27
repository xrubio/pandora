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

#ifndef __RasterAnalysisSelection_hxx__
#define __RasterAnalysisSelection_hxx__

#include <QWidget>
#include <ui_RasterAnalysisSelection.h>

namespace GUI
{

class RasterAnalysisSelection : public QWidget
{
	Q_OBJECT

	Ui::RasterAnalysisSelection _selection;

public:
	enum GlobalAnalysis 
	{
		eMean = 0,
		eSum = 1//,
	//	eStandardDeviation = 2
	};


public slots:
	void analysisSelected( const QString & option);
	void removeAnalysis();


public:
	RasterAnalysisSelection( QWidget * parent );
	virtual ~RasterAnalysisSelection();
	GlobalAnalysis getAnalysis() const;
signals:
	void removeAnalysis(QWidget *);
};


} // namespace GUI

#endif // __RasterAnalysisSelection_hxx__

