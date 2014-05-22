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
  
    HeatMap * heatMap = new HeatMap(0);
	connect(this, SIGNAL(updateXTicks(std::vector<float>)), heatMap, SLOT(setXValues(std::vector<float>)));
	connect(this, SIGNAL(updateYTicks(std::vector<float>)), heatMap, SLOT(setYValues(std::vector<float>)));
	connect(this, SIGNAL(updateVariables(std::vector< std::vector<float> >)), heatMap, SLOT(setVariables(std::vector< std::vector<float> >)));

    layout()->addWidget(heatMap);

    if(!loadGroupFile())
    {
        return;
    }

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

    _rows.clear();

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
        _rows.push_back(row);
    }

    std::cout << "number of rows: " << _rows.size() << std::endl;

    selectXAxis(0);
    selectYAxis(0);
    selectVariable(0);
    return true;
}

void HeatMapDialog::selectXAxis( int index )
{
    _xTicks.clear();
    if(_rows.size()==0)
    {
        return;
    }
    for(size_t i=0; i<_rows.size(); i++)
    {
        _xTicks.push_back(_rows.at(i).at(index));
    }
    std::sort(_xTicks.begin(), _xTicks.end());
    std::vector<float>::iterator it = std::unique(_xTicks.begin(), _xTicks.end());
    _xTicks.resize(std::distance(_xTicks.begin(), it));
    emit(updateXTicks(_xTicks));
}

void HeatMapDialog::selectYAxis( int index )
{
    _yTicks.clear();
    if(_rows.size()==0)
    {
        return;
    }
    for(size_t i=0; i<_rows.size(); i++)
    {
        _yTicks.push_back(_rows.at(i).at(index));
    }
    std::sort(_yTicks.begin(), _yTicks.end());
    std::vector<float>::iterator it = std::unique(_yTicks.begin(), _yTicks.end());
    _yTicks.resize(std::distance(_yTicks.begin(), it));
    emit(updateYTicks(_yTicks));
}

void HeatMapDialog::selectVariable( int index )
{
    if(_xTicks.size()==0 || _yTicks.size()==0)
    {
        return;
    }
    std::vector< std::vector<float> > values;

    values.resize(_xTicks.size());
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        values.at(i).resize(_yTicks.size());
    }

    int indexX = _heatMapDialog.xAxis->currentIndex();
    int indexY = _heatMapDialog.yAxis->currentIndex();

    std::cout << "begin rows" << std::endl;
    for(size_t i=0; i<_rows.size(); i++)
    {
        for(size_t j=0; j<_rows.at(i).size(); j++)
        {
            std::cout << " (" << i << "/" << j << ") = " << _rows.at(i).at(j);
        }
        std::cout << std::endl;
    }
    std::cout << "end rows" << std::endl;

    std::cout << "matrix of size: " << _xTicks.size() << "/" << _yTicks.size() << std::endl;
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        for(size_t j=0; j<_yTicks.size(); j++)
        {   
            int numRuns = 0;
            float value = 0.0f;
            // collect all data and divide by mean of all the rows that have this value
            std::cout << "checking values: " << _xTicks.at(i) << "/" << _yTicks.at(j) << " at indexes " << indexX << "/" << indexY << std::endl;
            for(auto row : _rows)
            {
                if(row.at(indexX)==_xTicks.at(i) && row.at(indexY)==_yTicks.at(j))
                {
                    std::cout << "oeoe" << std::endl;
                    numRuns++;
                    value += row.at(index);
                }
            }
            std::cout << "runs with x: " << _xTicks.at(i) << " and: " << _yTicks.at(j) << ": " << numRuns << std::endl;
            if(numRuns==0)
            {
                values.at(i).at(j) = 0;
            }
            else
            {
                values.at(i).at(j) = value/float(numRuns);
            }
        }
    }

    std::cout << "matrix for variable: " << index << " is: " << values.size() << "x" << values.at(0).size() << std::endl;
    for(size_t i=0; i<values.size(); i++)
    {
        for(size_t j=0; j<values.at(i).size(); j++)
        {
            std::cout << " (" << i << "/" << j << ") = " << values.at(i).at(j);
        }
        std::cout << std::endl;
    }

    emit(updateVariables(values));
}

} // namespace GUI

