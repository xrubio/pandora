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
#include <iostream>

namespace GUI
{

TimeSeriesDialog::TimeSeriesDialog(QWidget * parent, const std::string & groupFile) : QDialog(parent)
{
    setModal(false);
	_ts.setupUi(this);

    _model.loadGroupFile(groupFile);

    for( auto param : _model.params())
    {
        std::cout << "next param: " << param << std::endl;
    }

    for( auto result : _model.results())
    {
        std::cout << "next result: " << result << std::endl;
    }
    
    for( auto step : _model.timeSteps())
    {
        std::cout << "next step: " << step << std::endl;
    }

}

TimeSeriesDialog::~TimeSeriesDialog()
{
}

} // namespace GUI

