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

#ifndef __TraitAnalysisSelection_hxx__
#define __TraitAnalysisSelection_hxx__

#include <QWidget>
#include <ui_TraitAnalysisSelection.h>

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{

class TraitAnalysisSelection : public QWidget
{
	Q_OBJECT

	Ui::TraitAnalysisSelection _selection;

public:
	enum GlobalAnalysis 
	{
		eMean = 0,
		eSum = 1,
		eStandardDeviation = 2
	};


public slots:
	void analysisSelected( const QString & option);
	void removeAnalysis();

public:
	TraitAnalysisSelection( QWidget * parent, Engine::SimulationRecord * record, const std::string & type );
	virtual ~TraitAnalysisSelection();
	GlobalAnalysis getAnalysis() const;
	std::string getTrait() const;
signals:
	void removeAnalysis(QWidget *);
};


} // namespace GUI

#endif // __TraitAnalysisSelection_hxx__

