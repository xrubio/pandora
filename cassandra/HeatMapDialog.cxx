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

HeatMapDialog::HeatMapDialog(QWidget * parent, const std::string & groupFile ) : QDialog(parent), _groupFile(groupFile)
{
	setModal(false);
	_heatMapDialog.setupUi(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // //, QSizePolicy::Fixed);
    setSizePolicy(sizePolicy);

	connect(_heatMapDialog.xAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(selectXAxis(int)));
	connect(_heatMapDialog.yAxis, SIGNAL(currentIndexChanged(int)), this, SLOT(selectYAxis(int)));
	connect(_heatMapDialog.variable, SIGNAL(currentIndexChanged(int)), this, SLOT(selectVariable(int)));

    if(!loadGroupFile())
    {
        return;
    }
    HeatMap * heatMap = new HeatMap(0);
	connect(this, SIGNAL(updateXTicks(std::vector<float>)), heatMap, SLOT(setXValues(std::vector<float>)));
	connect(this, SIGNAL(updateYTicks(std::vector<float>)), heatMap, SLOT(setYValues(std::vector<float>)));

    layout()->addWidget(heatMap);
}

HeatMapDialog::~HeatMapDialog()
{
}

void HeatMapDialog::fillParams()
{
}

void HeatMapDialog::fillVariables()
{
}

bool HeatMapDialog::loadGroupFile()
{
    std::ifstream infile(_groupFile.c_str());
    std::string line;

    // header
    std::getline(infile, line);

    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;        
    
    // first token is run
    std::getline(iss, token, ';');
    while(std::getline(iss, token, ';'))
    {
        tokens.push_back(token);
    }

    QStringList headers;
    for(size_t i=0; i<tokens.size(); i++)
    {
        headers << tokens.at(i).c_str();
    }
    _heatMapDialog.xAxis->addItems(headers);
    _heatMapDialog.yAxis->addItems(headers);
    _heatMapDialog.variable->addItems(headers);

    _values.clear();

    while(std::getline(infile, line))
    {
        std::vector<float> row;
        std::istringstream iss(line);
        std::string token;

        // first token is run
        std::getline(iss, token, ';');

        while(std::getline(iss, token, ';'))
        {
            QString foo(token.c_str());
            row.push_back(foo.toFloat());
        }
        _values.push_back(row);
    }
    return true;
}

void HeatMapDialog::selectXAxis( int index )
{
    std::vector<float> values;
    if(_values.size()==0)
    {
        return;
    }
    for(size_t i=0; i<_values.size(); i++)
    {
        values.push_back(_values.at(i).at(index));
    }
    std::cout << "new x: " << index << " values: " << values.size() << std::endl;
    emit(updateXTicks(values));
}

void HeatMapDialog::selectYAxis( int index )
{
    std::vector<float> values;
    if(_values.size()==0)
    {
        return;
    }
    for(size_t i=0; i<_values.size(); i++)
    {
        values.push_back(_values.at(i).at(index));
    }
    std::cout << "new y: " << index << " values: " << values.size() << std::endl;
    emit(updateYTicks(values));
}

void HeatMapDialog::selectVariable( int index )
{
    std::cout << "new variable: " << index << std::endl;
}

} // namespace GUI

