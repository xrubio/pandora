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

#include <RasterSelection.hxx>
#include <SimulationRecord.hxx>

#include <algorithm>

#include <iostream>

#include <QDropEvent>

namespace GUI
{

RasterSelection::RasterSelection(QWidget * parent ) : QListWidget(parent), _simulationRecord(0)
{
    setDragDropMode(QAbstractItemView::InternalMove);
	setMouseTracking(true);
    connect(this,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(updateRastersSlot(QListWidgetItem*)));
}

RasterSelection::~RasterSelection()
{
}

void RasterSelection::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_simulationRecord = simulationRecord;
	clear();
	if(!simulationRecord)
	{
		update();
		return;
	}
	for(Engine::SimulationRecord::RasterMap::const_iterator it=_simulationRecord->beginRasters(); it!=_simulationRecord->endRasters(); it++)
	{
		addItem(it->first.c_str());
	}
	for(Engine::SimulationRecord::StaticRasterMap::const_iterator it=_simulationRecord->beginStaticRasters(); it!=_simulationRecord->endStaticRasters(); it++)
	{
		addItem(it->first.c_str());
	}
	setCurrentRow(0);
    for(int i=0; i<count(); i++)
    {
        item(i)->setFlags(item(i)->flags()|Qt::ItemIsUserCheckable);
        item(i)->setCheckState(Qt::Checked);
    }
	update();
	updateRasters();
}

void RasterSelection::updateRasters()
{
	_rasterList.clear();
	_rasterView.clear();
	std::list<std::string> items;
	for(int i=0; i<count(); i++)
    {
		std::string newItem(item(i)->text().toStdString());
		_rasterList.push_back(newItem);
        if (item(i)->checkState()==Qt::Checked)
		{
			_rasterView.push_back(true);
        }
		else
		{
			_rasterView.push_back(false);
		}
    }
	emit rastersRearranged(_rasterList, _rasterView);
}

void RasterSelection::dropEvent( QDropEvent * event )
{
	QListWidget::dropEvent(event);
	updateRasters();
}

void RasterSelection::updateRastersSlot(QListWidgetItem*)
{
    updateRasters();
}


const std::list<std::string> & RasterSelection::getRasterList() const
{
	return _rasterList;
}

const std::list<bool> & RasterSelection::getRasterView() const
{
	return _rasterView;
}

} // namespace GUI

