/*
 * Copyright (c) 2014
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

#include <OpenMPSingleNode.hxx>
#include <Agent.hxx>
#include <Logger.hxx>
#include <Config.hxx>
#include <Exception.hxx>
#include <boost/chrono.hpp>

namespace Engine
{

OpenMPSingleNode::OpenMPSingleNode() : _serializer(*this)
{
}

OpenMPSingleNode::~OpenMPSingleNode()
{
}

void OpenMPSingleNode::init( int argc, char *argv[] )
{
	_timer.start();
	_boundaries._origin = Point2D<int>(0,0);
	_boundaries._size = _world->getConfig().getSize();
	std::cout << "simulation: " << _id << " of: " << _numTasks << " initialized" << std::endl;
}

void OpenMPSingleNode::initData()
{
	// serializer init
	_serializer.init(*_world);
	std::stringstream logName;
	logName << "simulation_" << _id;
	log_INFO(logName.str(), "finished init at: "  << getWallTime());
}


void OpenMPSingleNode::executeAgents()
{
	std::stringstream logName;
	logName << "simulation_" << getId();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _world->getCurrentStep() << " executing sections");

	//AgentsVector agentsToExecute{_world->beginAgents(), _world->endAgents() };
	AgentsVector agentsToExecute;
	for(AgentsList::iterator it=_world->beginAgents(); it!=_world->endAgents(); it++)
	{
		agentsToExecute.push_back(*it);
	}
	std::random_shuffle(agentsToExecute.begin(), agentsToExecute.end());

#ifndef PANDORAEDEBUG
	// shared memory distibution for read-only planning actions, disabled for extreme debug
	#pragma omp parallel for
#endif
	for(size_t i=0; i<agentsToExecute.size(); i++)
	{
		Agent * agent = agentsToExecute[i].get();
		agent->updateKnowledge();
		agent->selectActions();
	}	
	
	for(size_t i=0; i<agentsToExecute.size(); i++)
	{
		Agent * agent = agentsToExecute[i].get();
		agent->executeActions();
		agent->updateState();
	}
	log_DEBUG(logName.str(), getWallTime() << " executed step: " << _world->getCurrentStep() << " executed agents: " << agentsToExecute.size() << " total agents: " << std::distance(_world->beginAgents(), _world->endAgents()));
}

void OpenMPSingleNode::finish()
{	
	std::stringstream logName;
	logName << "simulation_" << _id;

	_serializer.finish();

	log_INFO(logName.str(), getWallTime() << " simulation finished");
	_timer.stop();
}

Agent * OpenMPSingleNode::getAgent( const std::string & id )
{	
	AgentsList::iterator it = getAgentIterator(id);
	if(it!=_world->endAgents() && (*it)->exists())
	{
		return it->get();
	}
	return 0;
}

void OpenMPSingleNode::removeAgent( Agent * agent )
{
	AgentsList::iterator it = getAgentIterator(agent->getId());
	if(it!=_world->endAgents())
	{
		_removedAgents.push_back(*it);
		return;
	}
	std::stringstream oss;
	oss << "Scheduler::removeAgent - agent: " << agent << " not found";
	throw Exception(oss.str());
}

AgentsList::iterator OpenMPSingleNode::getAgentIterator( const std::string & id )
{
	for(AgentsList::iterator it=_world->beginAgents(); it!=_world->endAgents(); it++)
	{
		Agent * agent = it->get();
		if(agent->getId()==id)
		{
			return it;
		}
	}
	return _world->endAgents();
}

void OpenMPSingleNode::removeAgents()
{
	AgentsList::iterator it=_removedAgents.begin();
	while(it!=_removedAgents.end())
	{
		Agent * agent = it->get();
		AgentsList::iterator itAg = getAgentIterator(agent->getId());
		if(itAg==_world->endAgents())
		{
			std::stringstream oss;
			oss << "OpenMPSingleNode ::removeAgents - agent: " << agent << " not found";
			throw Exception(oss.str());
			return;
		}
		_world->eraseAgent(itAg);
		it = _removedAgents.erase(it);
	}
	_removedAgents.clear();
}

AgentsVector OpenMPSingleNode::getAgent( const Point2D<int> & position, const std::string & type )
{
	AgentsVector result;
	for(AgentsList::iterator it=_world->beginAgents(); it!=_world->endAgents(); it++)
	{
		if((*it)->getPosition().isEqual(position))
		{
			if(type.compare("all")==0 || (*it)->isType(type))
			{
				result.push_back(*it);
			}
		}
	}
	return result;
}

Point2D<int> OpenMPSingleNode::getRandomPosition() const
{
	Engine::Point2D<int> pos(GeneralState::statistics().getUniformDistValue(0,_boundaries._size._width-1), GeneralState::statistics().getUniformDistValue(0,_boundaries._size._height-1));
	return pos;
}

double OpenMPSingleNode::getWallTime() const
{
	boost::chrono::duration<double> seconds = boost::chrono::nanoseconds(_timer.elapsed().wall);
	return seconds.count();
}

void OpenMPSingleNode::addStringAttribute( const std::string & type, const std::string & key, const std::string & value )
{
	_serializer.addStringAttribute(type, key, value);
}

void OpenMPSingleNode::addFloatAttribute( const std::string & type, const std::string & key, float value )
{
	_serializer.addFloatAttribute(type, key, value);
}

void OpenMPSingleNode::addIntAttribute( const std::string & type, const std::string & key, int value )
{
	_serializer.addIntAttribute(type, key, value);
}

void OpenMPSingleNode::serializeAgents( const int & step )
{
	_serializer.serializeAgents(step, _world->beginAgents(), _world->endAgents());
}

void OpenMPSingleNode::serializeRasters( const int & step )
{
	_serializer.serializeRasters(step);
}

int OpenMPSingleNode::countNeighbours( Agent * target, const double & radius, const std::string & type )
{
	return for_each(_world->beginAgents(), _world->endAgents(), aggregatorCount<std::shared_ptr<Agent> >(radius,*target, type))._count;
}

AgentsVector OpenMPSingleNode::getNeighbours( Agent * target, const double & radius, const std::string & type )
{
	AgentsVector agentsVector = for_each(_world->beginAgents(), _world->endAgents(), aggregatorGet<std::shared_ptr<Agent> >(radius,*target, type))._neighbors;
	std::random_shuffle(agentsVector.begin(), agentsVector.end());
	return agentsVector;
}
	
void OpenMPSingleNode::setValue( DynamicRaster & raster, const Point2D<int> & position, int value )
{
	raster.setValue(position, value);
}

int OpenMPSingleNode::getValue( const DynamicRaster & raster, const Point2D<int> & position ) const
{
	return raster.getValue(position);
}

void OpenMPSingleNode::setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value )
{
	raster.setMaxValue(position, value);
}

int OpenMPSingleNode::getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const
{
	return raster.getMaxValue(position);
}

} // namespace Engine

