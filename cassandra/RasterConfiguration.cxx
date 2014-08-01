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

#include <RasterConfiguration.hxx>
#include <ColorSelector.hxx>
#include <DefaultColorSelector.hxx>
#include <iostream>

namespace GUI
{

RasterConfiguration::RasterConfiguration( const int & minValue, const int & maxValue, bool init ) : _colorSelector(0), _minValue(minValue), _maxValue(maxValue), _transparentEnabled(false), _transparentValue(0), _elevationRaster("none (use plane)"), _cellResolution(1.0f), _elevationExaggeration(1.0f), _offset(0.0f, 0.0f, 0.0f), _lod(25), _hasElevationRaster(false), _showValues(false), _showBorders(false)
{
	resetColorRamp();

	if(!init)
	{
		return;
	}

    int numSteps = std::min(5, maxValue+1-_minValue);
	float range = (float)(_maxValue + 1 - _minValue)/numSteps;
    
     _colorSelector->setColor(QColor("#FDECA6"),0);

    int i=1;
    if(i>=numSteps)
    {
        return;
    }
    std::cout << "\tadding break: " << minValue+range*i << " with range: " << range << std::endl;
    _colorSelector->addBreak(minValue+range*i);
    _colorSelector->setColor(QColor("#E6D298"),i);
    i++;

    if(i>=numSteps)
    {
        return;
    }
    std::cout << "\tadding break: " << minValue+range*i << " with range: " << range << std::endl;
    _colorSelector->addBreak(minValue+range*i);
    _colorSelector->setColor(QColor("#CFB98B"),i);
    i++;

    if(i>=numSteps)
    {
        return;
    }
    std::cout << "\tadding break: " << minValue+range*i << " with range: " << range << std::endl;
    _colorSelector->addBreak(minValue+range*i);
    _colorSelector->setColor(QColor("#B89F7D"),i);
    i++;

    if(i>=numSteps)
    {
        return;
    }
    std::cout << "\tadding break: " << minValue+range*i << " with range: " << range << std::endl;
    _colorSelector->addBreak(minValue+range*i);
    _colorSelector->setColor(QColor("#A1856F"),i);

    /*
	// basic behavior, 5 different random values
	int numSteps = std::min(5, maxValue+1-_minValue);
	float range = (float)(_maxValue + 1 - _minValue)/numSteps;
	float rangeColor = 255.0f/(numSteps-1);
	
	std::cout << "num steps: " << numSteps << " min/max: " << minValue << "/" << maxValue << " range: " << range << " and range color: " << rangeColor << std::endl;
	// first color = grey, not black
	_colorSelector->setColor(QColor(100,100,100), 0);
	for(int i=1; i<numSteps; i++)
	{
		_colorSelector->addBreak(minValue+range*i);
		//std::cout << "\tadding break: " << minValue+range*i << " with color: " << rangeColor*i << std::endl;
		_colorSelector->setColor(QColor(0,rangeColor*i,0), i);
	}
    */
}

RasterConfiguration::RasterConfiguration( const RasterConfiguration & prototype ) : _colorSelector(0), _minValue(prototype.getMinValue()), _maxValue(prototype.getMaxValue()), _transparentEnabled(prototype.isTransparentEnabled()), _transparentValue(prototype.getTransparentValue()), _elevationRaster(prototype.getElevationRaster()), _cellResolution(prototype.getCellResolution()), _elevationExaggeration(prototype.getElevationExaggeration()), _offset(prototype.getOffset()), _lod(prototype.getLOD()), _hasElevationRaster(prototype.hasElevationRaster()), _showValues(prototype.showValues()), _showBorders(prototype.showBorders())
{
	_colorSelector =  prototype.getColorRamp().copy();
}

RasterConfiguration::~RasterConfiguration()
{
	delete _colorSelector;
}

void RasterConfiguration::resetColorRamp()
{
	if(_colorSelector)
	{
		delete _colorSelector;
	}
	_colorSelector = new DefaultColorSelector(_minValue,_maxValue);
}

ColorSelector & RasterConfiguration::getColorRamp()
{
	return *_colorSelector;
}

const ColorSelector & RasterConfiguration::getColorRamp() const
{
	return *_colorSelector;
}

const int & RasterConfiguration::getMinValue() const
{
	return _minValue;
}

const int & RasterConfiguration::getMaxValue() const
{
	return _maxValue;
}

const bool & RasterConfiguration::isTransparentEnabled() const
{
	return _transparentEnabled;
}

void RasterConfiguration::setTransparentEnabled( const bool & transparentEnabled )
{
	_transparentEnabled = transparentEnabled;
}

const int & RasterConfiguration::getTransparentValue() const
{
	return _transparentValue;
}

void RasterConfiguration::setTransparentValue( const int & transparentValue )
{
	_transparentValue = transparentValue;
}

void RasterConfiguration::setElevationRaster( const std::string & elevationRaster )
{
	_elevationRaster = elevationRaster;
	if(_elevationRaster.compare("none (use plane)")==0)
	{
		_hasElevationRaster = false;
	}
	else
	{
		_hasElevationRaster = true;
	}
}

const std::string & RasterConfiguration::getElevationRaster() const
{
	return _elevationRaster;
}

void RasterConfiguration::setElevationExaggeration( float elevationExaggeration)
{
	_elevationExaggeration = elevationExaggeration;
}

float RasterConfiguration::getElevationExaggeration() const
{
	return _elevationExaggeration;
}

void RasterConfiguration::setCellResolution( float cellResolution)
{
	_cellResolution = cellResolution;
}

float RasterConfiguration::getCellResolution() const
{
	return _cellResolution;
}

void RasterConfiguration::setOffset( const Engine::Point3D<float> & offset)
{
	_offset = offset;
}

const Engine::Point3D<float> & RasterConfiguration::getOffset() const
{
	return _offset;
}

void RasterConfiguration::setLOD( int lod )
{
	_lod = lod;
}

int RasterConfiguration::getLOD() const
{
	return _lod;
}

bool RasterConfiguration::hasElevationRaster() const
{
	return _hasElevationRaster;
}

} // namespace GUI

