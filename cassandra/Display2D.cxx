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

#include <Display2D.hxx>
#include <DynamicRaster.hxx>
#include <SimulationRecord.hxx>
#include <AgentRecord.hxx>
#include <Exception.hxx>
#include <QPainter>
#include <QToolTip>
#include <QListWidgetItem>
#include <QPixmap>
#include <sstream>
#include <iostream>
#include <AgentConfiguration.hxx>
#include <RasterConfiguration.hxx>
#include <ProjectConfiguration.hxx>
#include <ColorSelector.hxx>
#include <algorithm>

#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace GUI
{

Display2D::Display2D( QWidget * parent) : QWidget(parent), _simulationRecord(0), _viewedStep(0), _zoom(1), _showAgents(true), _radiusSelection(7), _offset(0,0), _clickedPos(0,0), _type("unknown"), _state("unknown")
{
	setMouseTracking(true);
	setAutoFillBackground(true);
	QPalette p(palette());
	p.setColor(QPalette::Background, Qt::lightGray);
    setPalette(p);
    agentList = new QTreeWidget();
    connect(agentList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), parent, SLOT(show3Dagent(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(updateAgentsSelected(std::list<Engine::AgentRecord*>,Engine::SimulationRecord *)), parent, SLOT(updateAgentsSelected(std::list<Engine::AgentRecord*>,Engine::SimulationRecord *)));
}

Display2D::~Display2D()
{
}

void Display2D::resetView()
{
	_viewedStep = 0;
	_offset.setX(0);
	_offset.setY(0);

    if(_simulationRecord)
    {
	    _zoom = 600.0f/float(std::max(_simulationRecord->getSize()._width, _simulationRecord->getSize()._height));
    }
    else
    {
	    _zoom = 1.0f;
    }
}

void Display2D::setSimulationRecord( Engine::SimulationRecord * simulationRecord )
{
	_orderedRasters.clear();
	_simulationRecord = simulationRecord;
	resetView();
	update();
}

QSize Display2D::getRealSize() const
{
    if(!_simulationRecord)
    {
        return QSize(0,0);
    }
    return QSize(_simulationRecord->getSize()._width*_zoom, _simulationRecord->getSize()._height*_zoom);
}

void Display2D::paintEvent(QPaintEvent *event)
{
	if(!_simulationRecord || _simulationRecord->getLoadingPercentageDone()!=100.0f) //|| _orderedRasters.empty())
	{
		return;
	}
	QPixmap imageToDraw(getRealSize());
	//QImage imageToDraw(_simulationRecord->getSize()*_zoom, _simulationRecord->getSize()*_zoom, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&imageToDraw);
	QPen pen;
	pen.setWidth(_zoom);

	imageToDraw.fill(QColor(100,100,100));

	if(!_orderedRasters.empty())
	{
		Engine::StaticRaster & rasterTmp(_simulationRecord->getRasterTmp(*(_orderedRasters.begin()), _viewedStep));
		Engine::Size<int> size = rasterTmp.getSize();

		for(int i=0; i<size._width; i++)
		{
			for(int j=0; j<size._height; j++)
			{
				std::list<std::string>::const_iterator it =_orderedRasters.end();
				while(it!=_orderedRasters.begin())
				{
					it--;
					RasterConfiguration * rasterConfig = ProjectConfiguration::instance()->getRasterConfig(*it);
					Engine::StaticRaster & raster(_simulationRecord->getRasterTmp(*it, _viewedStep));
					int value = raster.getValue(Engine::Point2D<int>(i,j));
			
					if(rasterConfig->isTransparentEnabled() && value==rasterConfig->getTransparentValue())
					{
						continue;
					}

					if(raster.hasColorTable())
					{
						Engine::ColorEntry color = raster.getColorEntry(value);
						pen.setColor(QColor(color._r, color._g, color._b, color._alpha));
					}
					else
					{
						ColorSelector & colorSelector = rasterConfig->getColorRamp();
						pen.setColor(colorSelector.getColor(value));
					}
					painter.setPen(pen);
					painter.drawPoint(i*_zoom+_zoom/2,j*_zoom+_zoom/2);
					break;
				}
			}
		}
	}

	if(!_showAgents)
	{
		QPainter screenPainter(this);	
		screenPainter.save();
		screenPainter.drawPixmap(_offset, imageToDraw); //.scaled(_simulationRecord->getSize()*_zoom, _simulationRecord->getSize()*_zoom));
		screenPainter.restore();
		return;
	}

	painter.end();
	painter.begin(&imageToDraw);

	for(Engine::SimulationRecord::AgentTypesMap::const_iterator itType = _simulationRecord->beginTypes(); itType!=_simulationRecord->endTypes(); itType++)
	{
		AgentConfiguration * agentConfig = ProjectConfiguration::instance()->getAgentConfig(itType->first);
		for(Engine::SimulationRecord::AgentRecordsMap::const_iterator it= _simulationRecord->beginAgents(itType); it!=_simulationRecord->endAgents(itType); it++)
		{
			if(agentConfig->useIcon() && !agentConfig->getFileName2D().empty())
			{	
				Engine::AgentRecord * agent = it->second;
				bool exists = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), "exists");
				if(exists)
				{
					int x = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), "x");
					int y = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), "y");
					int size = (float)_zoom*agentConfig->getSize();
					int xPos = x*_zoom - size/2;						
					int yPos = y*_zoom - size/2;
					agentConfig->getIcon().paint(&painter, QRect(xPos, yPos, size, size));
				}
			}
			else
			{
				QColor colorToUse(agentConfig->getColor());
				Engine::AgentRecord * agent = it->second;
				bool exists = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), "exists");
				if(exists)
				{
					int x = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), "x");
					int y = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), "y");
					QBrush brush(Qt::SolidPattern);
					if(_state=="unknown")
					{
						brush.setColor(colorToUse);
                        painter.setPen(Qt::NoPen);
						painter.setBrush(brush);
					}
					else
					{
						// we put this call between try/catch in order to avoid crashes about painting state in agents that don't have it
						int value = 0;
						try
						{
							value = agent->getState(_viewedStep/_simulationRecord->getFinalResolution(), _state);
							int max = _simulationRecord->getMaxValueForState(_state);
							int min = _simulationRecord->getMinValueForState(_state);
							int diff = std::max(1,max - min);
							value = (value-min)*255/diff;
						}
						catch( Engine::Exception & exceptionThrown )
						{
						}
						brush.setColor(QColor(255,255-value,255-value));
						painter.setBrush(brush);
					}
					int size = agentConfig->getSize();
					int xPos = x*_zoom - ((size-1)*_zoom)/2;
					int yPos = y*_zoom - ((size-1)*_zoom)/2;
					painter.drawEllipse(xPos, yPos, size*_zoom, size*_zoom);
				}
			}
		}
	}
	QPainter screenPainter(this);	
	screenPainter.save();
	screenPainter.drawPixmap(_offset, imageToDraw); //.scaled(_simulationRecord->getSize()*_zoom, _simulationRecord->getSize()*_zoom));
	screenPainter.restore();
}

void Display2D::typeSelected( QListWidgetItem * item )
{
	_type = item->text().toStdString();
	_state = "unknown";
	// actualitzar estats
	update();
}
void Display2D::stateSelected( QListWidgetItem * item )
{
	_state = item->text().toStdString();
	if(_type!="unknown" && _state!="unknown")
	{
		emit calculateStatistics(_type,_state);
	}
	update();
}

void Display2D::rastersRearranged( std::list<std::string> items, std::list<bool> views)
{
	_orderedRasters.clear();

	std::list<bool>::const_iterator itView = views.begin();
	for(std::list<std::string>::const_iterator it=items.begin(); it!=items.end(); it++)
	{
		if((*itView)==true)
		{
			_orderedRasters.push_front(*it);
		}
		itView++;
	}
	update();
}

QSize Display2D::minimumSizeHint() const
{
	return QSize(200, 200);
}

QSize Display2D::sizeHint() const
{
	return QSize(800, 800);
}

void Display2D::viewedStepChangedSlot( int newViewedStep )
{
    setViewedStep(newViewedStep);
	update();
}

void Display2D::zoomIn()
{
	zoom(1);
}

void Display2D::zoomOut()
{
	zoom(-1);
}

void Display2D::zoom( float value )
{
	if(!_simulationRecord)
	{
		return;
	}
	_zoom *= 1+(value*0.1f);

	if(_zoom<0.001f)
	{
		_zoom = 0.001f;
	}
	else if(_zoom>100.0f)
	{
		_zoom = 100.0f;
	}
	update();
}

void Display2D::wheelEvent( QWheelEvent * event )
{
	float numDegrees = event->delta();
	numDegrees /= 120.0f;
	zoom(numDegrees);
}

void Display2D::mousePressEvent (QMouseEvent * event)
{
	if(event->button()==Qt::RightButton)
	{
		_clickedPos = event->globalPos() - _offset;
	}
}

void Display2D::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!_simulationRecord)
    {
        return;
    }
    Engine::Point2D<int> position(event->pos().x()-_offset.x(), event->pos().y()-_offset.y());
    // TODO program /= i *= in Engine::Point2D
    position._x /= _zoom;
    position._y /= _zoom;

    std::cout << "position._x " << position._x << " position._y " << position._y << std::endl;

    agentList->close();
    agentList->clear();

    std::list<Engine::AgentRecord*> agentsSelected;

    Engine::Point2D<int> positionAux(0,0);
	for(positionAux._x=position._x-_radiusSelection; positionAux._x<=position._x+_radiusSelection; positionAux._x++)
	{
		for(positionAux._y=position._y-_radiusSelection; positionAux._y<=position._y+_radiusSelection; positionAux._y++)
		{
			Engine::SimulationRecord::AgentRecordsVector agents = _simulationRecord->getAgentsAtPosition(_viewedStep/_simulationRecord->getFinalResolution(), positionAux);
			for(size_t i=0; i<agents.size(); i++)
			{
				Engine::AgentRecord * agentRecord = agents.at(i);
				if(!agentRecord->getState(_viewedStep/_simulationRecord->getFinalResolution(), "exists"))
				{
					continue;
				}
                std::string stateName = agentRecord->getId();

                if(agentList->findItems(stateName.c_str(),Qt::MatchFixedString).isEmpty())
				{
                    std::string info = agentRecord->getCompleteState(_viewedStep/_simulationRecord->getFinalResolution());
                    QTreeWidgetItem *item = new QTreeWidgetItem();
                    QTreeWidgetItem *item2 = new QTreeWidgetItem();
                    item->setText(0,QString(stateName.c_str()));
                    item2->setText(0,QString(info.c_str()));
                    item->addChild(item2);
                    agentList->addTopLevelItem(item);
                    agentsSelected.push_front(agentRecord);
                }
			}
        }
    }
    agentList->setAlternatingRowColors(true);
    agentList->resize(400,400);
    agentList->show();
    emit updateAgentsSelected(agentsSelected,_simulationRecord);
}

void Display2D::mouseMoveEvent (QMouseEvent * event)
{
	if(!_simulationRecord)
	{
		return;
	}
	if(event->buttons() & Qt::RightButton)
	{
		_offset = event->globalPos() - _clickedPos;
		update();
	}
}

std::string Display2D::getRasterToolTip( const Engine::Point2D<int> & position )
{
    std::stringstream toolTipString;
    std::list<std::string>::iterator it =_orderedRasters.end();
    while(it!=_orderedRasters.begin())
    {
        it--;
        Engine::StaticRaster & raster = _simulationRecord->getRasterTmp(*it, _viewedStep);
        toolTipString << *it << " in " << position << " : " << raster.getValue(position) << std::endl;
    }
    return toolTipString.str();
}

std::string Display2D::getAgentToolTip( const Engine::Point2D<int> & position )
{	
	std::stringstream toolTipString;
	Engine::SimulationRecord::AgentRecordsVector agents = _simulationRecord->getAgentsAtPosition(_viewedStep/_simulationRecord->getFinalResolution(), position);

	for(size_t i=0; i<agents.size(); i++)
	{
		Engine::AgentRecord * agentRecord = agents.at(i);
		if(agentRecord->getState(_viewedStep/_simulationRecord->getFinalResolution(), "exists")==false)
		{
			continue;
		}
		toolTipString << std::endl << agentRecord->getCompleteState(_viewedStep/_simulationRecord->getFinalResolution());
	}
	return toolTipString.str();
}


bool Display2D::event(QEvent *event)
{  
	if(event->type()==QEvent::ToolTip) 
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		Engine::Point2D<int> position(helpEvent->pos().x()-_offset.x(), helpEvent->pos().y()-_offset.y());
		// TODO program /= i *= in Engine::Point2D
		position._x /= _zoom;
		position._y /= _zoom;

		std::string finalToolTip("");
		std::stringstream posToolTip;
		posToolTip << "position: " << position << " zoom: " << _zoom;
		finalToolTip += posToolTip.str();
	
		if(!_simulationRecord || position._x<0 || position._y<0 || position._x>=_simulationRecord->getSize()._width || position._y>=_simulationRecord->getSize()._height)
		{
			QToolTip::showText(helpEvent->globalPos(), finalToolTip.c_str());
			return QWidget::event(event);
		}
		finalToolTip += getRasterToolTip(position);
		finalToolTip += getAgentToolTip(position);		
		
		QToolTip::showText(helpEvent->globalPos(), finalToolTip.c_str());
     }
     return QWidget::event(event);
}

void Display2D::showAgents()
{
	_showAgents = !_showAgents;
	update();
}

int Display2D::getRadiusSelection() const
{
	return _radiusSelection;
}
	
void Display2D::radiusSelectionModified(int radiusSelection)
{
	_radiusSelection = radiusSelection;
	std::cout << "radius selection: " << _radiusSelection << std::endl;
}

void Display2D::setViewedStep( int viewedStep )
{
    _viewedStep = viewedStep;
}

} // namespace GUI

