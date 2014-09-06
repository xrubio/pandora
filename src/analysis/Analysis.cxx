/*
 * Copyright (c) 2013
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#include <analysis/Analysis.hxx>

namespace PostProcess
{

Analysis::Analysis( const std::string & name, bool writeResults ) : _name(name), _writeResults(writeResults)
{
}

Analysis::~Analysis()
{
}

void Analysis::preProcess()
{
	for(unsigned i=0; i<_results.size(); i++)
	{
		_results.at(i) = 0.0f;
	}
}

void Analysis::setNumTimeSteps( int numTimeSteps )
{
	_results.clear();
	_results.resize(numTimeSteps);
}

const std::string & Analysis::getName() const
{
	return _name;
}
	
long double Analysis::getResult( int timeStep ) const
{
	return _results.at(timeStep);
}
	
} // namespace PostProcess

