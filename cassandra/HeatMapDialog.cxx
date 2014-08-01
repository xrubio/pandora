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

#include <HeatMapDialog.hxx>
#include <HeatMapView.hxx>
#include <QToolBar>
#include <QFileDialog>
#include <QPainter>

namespace GUI
{

HeatMapDialog::HeatMapDialog(QWidget * parent, const std::string & groupFile ) : QDialog(parent)
{
	setModal(false);
	_heatMapDialog.setupUi(this);

	connect(_heatMapDialog.xAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(selectXAxis(int)));
	connect(_heatMapDialog.yAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(selectYAxis(int)));
	connect(_heatMapDialog.variable, SIGNAL(currentIndexChanged(int)), this, SLOT(selectVariable(int)));
  
    HeatMapView * heatMap = new HeatMapView(0, _model);
	connect(this, SIGNAL(updateView()), heatMap, SLOT(updateView()));

    layout()->addWidget(heatMap);

    if(!_model.loadFile(groupFile))
    {
        return;
    }
    fillMenus();   
    
    QAction * screenshotAction = new QAction(QIcon(":share/cassandra/icons/screenshot.png"), tr("Take &Screenshot"), this);
    screenshotAction->setStatusTip(tr("Take a screenshot"));
	connect(screenshotAction, SIGNAL(triggered()), this, SLOT(takeScreenshot()));

    QAction * textAction = new QAction(QIcon(":share/cassandra/icons/text.png"), tr("Switch &Text"), this);
    textAction->setStatusTip(tr("Toggle text"));
	connect(textAction, SIGNAL(triggered()), heatMap, SLOT(switchText()));

    QToolBar * tools = new QToolBar(tr("Tools"));
    layout()->setMenuBar(tools);
	tools->addAction(screenshotAction);
	tools->addAction(textAction);
}

HeatMapDialog::~HeatMapDialog()
{
}

void HeatMapDialog::fillMenus()
{
    QStringList headers;
    for(size_t i=0; i<_model.variables().size(); i++)
    {
        headers << _model.variables().at(i).c_str();
        
    }
    _heatMapDialog.xAxis->addItems(headers);
    _heatMapDialog.yAxis->addItems(headers);
    _heatMapDialog.variable->addItems(headers);
}

void HeatMapDialog::selectXAxis( int index )
{
    if(_model.isEmpty())
    {
        return;
    }
    _model.updateXParam(index, _heatMapDialog.xAxis->currentText().toStdString());
    emit(updateView());
}

void HeatMapDialog::selectYAxis( int index )
{  
    if(_model.isEmpty())
    {
        return;
    }
    _model.updateYParam(index, _heatMapDialog.yAxis->currentText().toStdString());
    emit(updateView());
}

void HeatMapDialog::selectVariable( int index )
{
    if(_model.isEmpty())
    {
        return;
    }
    _model.updateVariable(index, _heatMapDialog.variable->currentText().toStdString());
    emit(updateView());
}

void HeatMapDialog::takeScreenshot()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	if(fileName.isEmpty())
	{
		return;
	}
    
    HeatMapView * view = findChild<HeatMapView*>();
    QImage img(view->size(), QImage::Format_RGB16);
    QPainter painter(&img);
    view->render(&painter);
    img.save(fileName);
}
} // namespace GUI

