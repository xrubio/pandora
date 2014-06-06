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

#ifndef __TimeSeriesDialog_hxx__
#define __TimeSeriesDialog_hxx__

#include <QDialog>
#include <ui_TimeSeriesDialog.h>
#include <TimeSeriesModel.hxx>

class QListWidget;

namespace GUI
{

class TimeSeriesDialog : public QDialog
{
	Q_OBJECT

	Ui::TimeSeriesDialog _ts;
    TimeSeriesModel _model;
    std::vector<QListWidget*> _listParams;
public:
	TimeSeriesDialog( QWidget * parent, const std::string & dirRuns );
	virtual ~TimeSeriesDialog();    
signals:
    void updateView();

public slots:
    void selectVariable( int index );
    void selectionChanged();
    void takeScreenshot();
};

} // namespace GUI

#endif // __TimeSeriesDialog_hxx__



