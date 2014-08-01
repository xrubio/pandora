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

#include <RasterConfigurator.hxx>
#include <ProjectConfiguration.hxx>
#include <ColorInterval.hxx>
#include <ColorSelector.hxx>

#include <iostream>

namespace GUI
{

RasterConfigurator::RasterConfigurator(QWidget * parent, const std::string & type, const std::list<std::string> & orderedRasters) : QDialog(parent), _type(type), _configuration(*ProjectConfiguration::instance()->getRasterConfig(type))
{
	setModal(true);
	_rasterConfig.setupUi(this);
	
	connect(_rasterConfig.transparentEnabled, SIGNAL(toggled(bool)), this, SLOT(transparentToggled(bool)));

	ColorSelector & colorRamp(_configuration.getColorRamp());
	
	for(unsigned int i=0; i<colorRamp.getNumIntervals(); i++)
	{
		ColorInterval * newColorInterval = new ColorInterval(this);
		int minValue = colorRamp.getBreak(i);
		int maxValue = colorRamp.getBreak(i+1)-1;

		if(i!=0)
		{
			newColorInterval->updateMinValue(colorRamp.getBreak(i-1));
		}
		else
		{
			newColorInterval->updateMinValue(colorRamp.getBreak(i)-1);
		}
		newColorInterval->setMaxValue(maxValue);
		newColorInterval->setMinValue(minValue);
		//std::cout << "new interval, min value: " << minValue << " and max Value: " << maxValue << std::endl;
		newColorInterval->colorSelected(colorRamp.getColor(minValue));
		
		_rasterConfig.colorRampLayout->addWidget(newColorInterval);
		connect(newColorInterval, SIGNAL(splitColorInterval()), this, SLOT(splitColorInterval()));
		connect(newColorInterval, SIGNAL(removeColorInterval()), this, SLOT(removeColorInterval( )));

		if(i==0)
		{
			newColorInterval->deactivateMinValue();
		}
		/*
		if(i==colorRamp.getNumIntervals()-1)
		{
			newColorInterval->deactivateMaxValue();
		}
		*/
	}

	for(unsigned int i=0; i<colorRamp.getNumIntervals()-1; i++)
	{
		QLayoutItem * child = _rasterConfig.colorRampLayout->itemAt(i);
		QLayoutItem * nextChild = _rasterConfig.colorRampLayout->itemAt(i+1);

		ColorInterval * interval = (ColorInterval*)child->widget();
		ColorInterval * nextInterval = (ColorInterval*)nextChild->widget();
		interval->connectToNext(nextInterval);
		// update minimums (maximums were already updated while calling at setMaximum)
		nextInterval->updateMinValue(interval->getMinValue());
	}

	_rasterConfig.transparentValue->setMinimum(_configuration.getMinValue());
	_rasterConfig.transparentValue->setMaximum(_configuration.getMaxValue());
	_rasterConfig.transparentValue->setValue(_configuration.getTransparentValue());
	_rasterConfig.transparentEnabled->setChecked(_configuration.isTransparentEnabled());
	if(!_configuration.isTransparentEnabled())
	{
		_rasterConfig.transparentValue->setEnabled(false);
	}

	// load rasters
	_rasterConfig.elevationRaster->addItem("none (use plane)");
	//std::cout << "active: " << _configuration.getElevationRaster() << " deform: " << _configuration.getElevationExaggeration() << std::endl;

	for(std::list<std::string>::const_iterator it=orderedRasters.begin(); it!=orderedRasters.end(); it++)
	{
		_rasterConfig.elevationRaster->addItem((*it).c_str());
		if(_configuration.getElevationRaster().compare(*it)==0)
		{
			_rasterConfig.elevationRaster->setCurrentIndex(_rasterConfig.elevationRaster->count()-1);
		}
	}

	_rasterConfig.cellResolution->setValue(_configuration.getCellResolution());
	_rasterConfig.elevationExaggeration->setValue(_configuration.getElevationExaggeration());


	_rasterConfig.xOffset->setValue(_configuration.getOffset()._x);
	_rasterConfig.yOffset->setValue(_configuration.getOffset()._y);
	_rasterConfig.zOffset->setValue(_configuration.getOffset()._z);

	_rasterConfig.levelOfDetail->setValue(_configuration.getLOD());

    if(_configuration.showValues())
    {
        _rasterConfig.showValues->setChecked(true);
    }
    else
    {
        _rasterConfig.showValues->setChecked(false);
    }
    if(_configuration.showBorders())
    {
        _rasterConfig.showBorders->setChecked(true);
    }
    else
    {
        _rasterConfig.showBorders->setChecked(false);
    }
	show();
}

RasterConfigurator::~RasterConfigurator()
{
}

void RasterConfigurator::accept()
{
	_configuration.resetColorRamp();
	ColorSelector & selector = _configuration.getColorRamp();

	for(int i=0; i<_rasterConfig.colorRampLayout->count(); i++)
	{
		ColorInterval * interval = (ColorInterval*)_rasterConfig.colorRampLayout->itemAt(i)->widget();
		// we must avoid the first break (as it is already created)
		if(i!=0) //_rasterConfig.colorRampLayout->count()-1)
		{
			selector.addBreak(interval->getMinValue());
		}
		selector.setColor(interval->getColor(), i);
	}

	_configuration.setTransparentValue(_rasterConfig.transparentValue->value());
	_configuration.setTransparentEnabled(_rasterConfig.transparentEnabled->isChecked());


	// 3D
	_configuration.setElevationRaster(_rasterConfig.elevationRaster->currentText().toStdString());
	_configuration.setCellResolution(_rasterConfig.cellResolution->value());
	_configuration.setElevationExaggeration(_rasterConfig.elevationExaggeration->value());
    _configuration.showValues(_rasterConfig.showValues->isChecked());
    _configuration.showBorders(_rasterConfig.showBorders->isChecked());

	Engine::Point3D<float> offset(_rasterConfig.xOffset->value(), _rasterConfig.yOffset->value(), _rasterConfig.zOffset->value());
	_configuration.setOffset(offset);

	_configuration.setLOD(_rasterConfig.levelOfDetail->value());

	emit rasterConfigured(_type, _configuration);
	close();
}

void RasterConfigurator::splitColorInterval()
{
	// maximum of 10 intervals
	if(_rasterConfig.colorRampLayout->count()>9)
	{
		return;
	}
	
	// if it has only 1 value it can't be split
	ColorInterval * splitted = (ColorInterval*)sender();
	//std::cout << "split from: " << splitted->getMaxValue() << " to: " << splitted->getMinValue() << std::endl;
	// if min==max it can't be splitted
	if(splitted->getMaxValue()-splitted->getMinValue()==0)
	{
		return;
	}

	int index = _rasterConfig.colorRampLayout->indexOf(splitted);
	ColorInterval * newColorInterval = new ColorInterval(this);
	
	connect(newColorInterval, SIGNAL(splitColorInterval()), this, SLOT(splitColorInterval()));
	connect(newColorInterval, SIGNAL(removeColorInterval()), this, SLOT(removeColorInterval( )));

	// value configuration
	int range = splitted->getMaxValue() - splitted->getMinValue();
	newColorInterval->setMaxValue(splitted->getMaxValue());

	splitted->connectToNext(newColorInterval);


	//newColorInterval->connectToNext();
		
	//splitted->disconnectMaxValue();
	splitted->setMaxValue(splitted->getMinValue()+range/2);
	newColorInterval->setMinValue(splitted->getMaxValue()+1);

	//splitted->adjustMaxValue(newColorInterval->getMaxValue()-1);
	//newColorInterval->adjustMinValue(splitted->getMinValue()+1);
	
	// color and signals config
	// if no more intervals, just copy the color
	QLayoutItem * childToRemove = _rasterConfig.colorRampLayout->itemAt(index+1);
	if(!childToRemove)
	{
		newColorInterval->setColor(splitted->getColor());
	}
	// on the contrary, mix both colors
	else
	{
		ColorInterval * next = (ColorInterval*)childToRemove->widget();
		QColor before(splitted->getColor());
		QColor after(next->getColor());

		QColor final((before.red()+after.red())/2, (before.green()+after.green())/2, (before.blue()+after.blue())/2);
		newColorInterval->setColor(final);
	
		//next->disconnectMinValue();
		newColorInterval->connectToNext(next);
	}
	// connection of color intervals
	//splitted->connectToNext(newColorInterval);

	// if it was the last interval, activate max and deactivate max of new one
	/*
	if(index==_rasterConfig.colorRampLayout->count()-1)
	{
		splitted->activateMaxValue();
		newColorInterval->deactivateMaxValue();
	}
	*/
	_rasterConfig.colorRampLayout->insertWidget(index+1, newColorInterval);
}

void RasterConfigurator::removeColorInterval()
{
	int index = _rasterConfig.colorRampLayout->indexOf((QWidget*)sender());

	// at least 1 color interval is required, so the first one is never removed
	if(index==0)
	{
		return;
	}
	
	QLayoutItem * childToRemove = _rasterConfig.colorRampLayout->itemAt(index);
	QLayoutItem * childToExpand = _rasterConfig.colorRampLayout->itemAt(index-1);

	// we adjust the max value of the interval before the one to be removed
	ColorInterval * colorIntervalToRemove = (ColorInterval*)childToRemove->widget();
	ColorInterval * colorInterval = (ColorInterval*)childToExpand->widget();
	//colorInterval->disconnectMaxValue();
	colorInterval->setMaxValue(colorIntervalToRemove->getMaxValue());

	QLayoutItem * childNext = _rasterConfig.colorRampLayout->itemAt(index);
	if(childNext)
	{
		ColorInterval * next = (ColorInterval*)childNext->widget();
		//next->disconnectMinValue();
		colorInterval->connectToNext(next);
		next->updateMinValue(colorInterval->getMinValue());
	}	
	
	// if it was the last interval, deactivate max of new one
	/*
	if(index==_rasterConfig.colorRampLayout->count()-1)
	{
		colorInterval->deactivateMaxValue();
	}
	*/

	// finally we remove the interval
	_rasterConfig.colorRampLayout->removeItem(childToRemove);
	delete colorIntervalToRemove;
	delete childToRemove;
}
	
void RasterConfigurator::transparentToggled( bool checked )
{
	_rasterConfig.transparentValue->setEnabled(checked);
}

} // namespace GUI

