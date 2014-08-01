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

#include <TimeSeriesDialog.hxx>
#include <TimeSeriesView.hxx>
#include <QListWidget>
#include <iostream>
#include <QToolBar>
#include <QFileDialog>
#include <QPainter>

namespace GUI
{

TimeSeriesDialog::TimeSeriesDialog(QWidget * parent, const std::string & groupFile) : QDialog(parent)
{
    setModal(false);
	_ts.setupUi(this);

    _model.loadGroupFile(groupFile);

    size_t i =0;
    for( auto param : _model.paramNames())
    {
        QGroupBox * newParam = new QGroupBox();
        newParam->setTitle(param.c_str());

        QListWidget * listValues = new QListWidget(0);
        listValues->setSelectionMode(QAbstractItemView::MultiSelection);

        std::vector<float> paramValues;
        for( size_t run=0; run<_model.numRuns(); run++)
        {
            paramValues.push_back(_model.params(run, i));
        }
        std::sort(paramValues.begin(), paramValues.end());
        std::vector<float>::iterator it = std::unique(paramValues.begin(), paramValues.end());
        paramValues.resize(std::distance(paramValues.begin(), it));

        for( size_t value=0; value<paramValues.size(); value++)
        {
            listValues->addItem(QString::number(paramValues.at(value)));
        }
        
        connect(listValues, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
        QVBoxLayout * vbox = new QVBoxLayout;
        vbox->addWidget(listValues);
        _listParams.push_back(listValues);
        newParam->setLayout(vbox);
        _ts.params->layout()->addWidget(newParam);
        i++;
    }

    QStringList results;
    for( auto result : _model.resultNames())
    {
        results.push_back(result.c_str());
    }
    _ts.result->addItems(results);
    
    TimeSeriesView * timeSeriesView = new TimeSeriesView(0, _model);
    connect(this, SIGNAL(updateView()), timeSeriesView, SLOT(updateView()));
    layout()->addWidget(timeSeriesView);
	
    connect(_ts.result, SIGNAL(currentIndexChanged(int)), this, SLOT(selectVariable(int)));
    selectVariable(0);

    QAction * screenshotAction = new QAction(QIcon(":share/cassandra/icons/screenshot.png"), tr("Take &Screenshot"), this);
    screenshotAction->setStatusTip(tr("Take a screenshot"));
	connect(screenshotAction, SIGNAL(triggered()), this, SLOT(takeScreenshot()));

    QToolBar * tools = new QToolBar(tr("Tools"));
    layout()->setMenuBar(tools);
	tools->addAction(screenshotAction);


}

TimeSeriesDialog::~TimeSeriesDialog()
{
}

void TimeSeriesDialog::selectVariable( int index )
{
    _model.selectResult(index);
    emit(updateView());
}

void TimeSeriesDialog::selectionChanged()
{
    for(size_t i=0; i<_listParams.size(); i++)
    {
        std::vector<float> selectedValues;
        QListWidget * paramList = _listParams.at(i);
        for(int j=0; j<paramList->count(); j++)
        {
            if(!paramList->item(j)->isSelected())
            {
                continue;
            }
            selectedValues.push_back(paramList->item(j)->text().toFloat());
        }
        _model.setSelectedValues(i, selectedValues);
    }
    emit(updateView());
}   

void TimeSeriesDialog::takeScreenshot()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	if(fileName.isEmpty())
	{
		return;
	}
    
    TimeSeriesView * view = findChild<TimeSeriesView *>();
    QImage img(view->size(), QImage::Format_RGB16);
    QPainter painter(&img);
    view->render(&painter);
    img.save(fileName);
}


} // namespace GUI

