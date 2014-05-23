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
#include <HeatMap.hxx>
#include <fstream>

namespace GUI
{

HeatMapDialog::HeatMapDialog(QWidget * parent, const std::string & groupFile, HeatMapModel & model ) : QDialog(parent)
{
	setModal(false);
	_heatMapDialog.setupUi(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);

	connect(_heatMapDialog.xAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(selectXAxis(int)));
	connect(_heatMapDialog.yAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(selectYAxis(int)));
	connect(_heatMapDialog.variable, SIGNAL(currentIndexChanged(int)), this, SLOT(selectVariable(int)));
  
    HeatMap * heatMap = new HeatMap(0, _model);
	connect(this, SIGNAL(updateView(), heatMap, SLOT(updateView())));

    layout()->addWidget(heatMap);

    if(!_model.loadFile(groupFile))
    {
        return;
    }
    fillMenus();
}

HeatMapDialog::~HeatMapDialog()
{
}

bool HeatMapDialog::fillMenus()
{
    QStringList headers;
    for(size_t i=0; i<_model.variables().size(); i++)
    {
        headers << _model.variables().at(i).c_str();
        
    }
    _heatMapDialog.xAxis->addItems(headers);
    _heatMapDialog.yAxis->addItems(headers);
    _heatMapDialog.variable->addItems(headers);

    return true;
}

void HeatMapDialog::selectXAxis( int index )
{
    _model.updateXParam(index);
    emit(updateView());
}

void HeatMapDialog::selectYAxis( int index )
{  
    _model.updateYParam(index);
    emit(updateView());
}

void HeatMapDialog::selectVariable( int index )
{
    _model.updateVariable(index);
    emit(updateView());
}

} // namespace GUI

