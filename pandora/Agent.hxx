
/*
 * Copyright (c) 2012
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

#ifndef __Agent_hxx__
#define __Agent_hxx__

#include <Point2D.hxx>
#include <World.hxx>

#include <string>

//#include <hdf5.h"

namespace Engine
{
class Action;

//! Base class for all agents 
/*!
  This class encapsulates all the needed code to create a standard agent. Apart from id, main important attributes are the Position 
  of the agent inside space (in global coords) and an instance to the World that owns the agent.
*/

class Agent
{
public:
	typedef std::list<std::string> AttributesList;
	
private:
	// list of attributes of an agent
	AttributesList _intAttributes;
	AttributesList _stringAttributes;

protected:
	/** Agent identifier **/
	std::string _id;
	/** Flag to control if agent is "dead" or alive". it is used in analyzer in order to know if agent must be painted **/
	bool _exists;
	/** Position of the agent, in global coordinates **/
	Point2D<int> _position;
	/** Pointer to the world that owns this agent **/
	World * _world;
	
	void serializeAttribute( const std::string & name, const int & value );
	void serializeAttribute( const std::string & name, const std::string & value );
	
	std::list<Engine::Action*> _actions;

public:

	void registerIntAttribute(const std::string & name ) { _intAttributes.push_back(name); }
	void registerStringAttribute(const std::string & name ) { _stringAttributes.push_back(name); }
	
	//! Standard constructor.
    /*! 
	  \param id id of the agent. It must be unique in the entire simulation.
    */
	Agent( const std::string & id );
	// els atribs de dalt segueixen : atribut\n comentari
	// perque els mètodes segueixen : comment\n metode ????
	// proposta : usar asserts 'require' i 'ensure'
	// exemple : 'require' afegit
	//! Standard constructor.
	/*! require :  
	  \param id id of the agent. It must be unique in the entire simulation.
	*/
	
	virtual ~Agent();

	const std::string & getId() const;

	bool exists() const;
	void setExists(bool exists);

	const Point2D<int> & getPosition() const;
	void setPosition( const Point2D<int> & position );

	// delete agent from world
	void remove();
	void setWorld( World * world );
	World * getWorld();
	World & getWorldRef();
	const World * getWorld() const;

	// method to locate the agent to a valid random position of _world
	void setRandomPosition();

	friend std::ostream & operator<<( std::ostream & stream, Agent * agent );
	friend std::ostream & operator<<( std::ostream & stream, Agent & agent );

	// TODO fix? when an agent is modified we need to alert the original in case of being an overlap copy
	//void interactionWithAgent(Agent * agent);
	virtual void * fillPackage() = 0;
	// this function returns true if the type of the agent is the one passed by reference
	bool isType( const std::string & type ) const;
	std::string getType() const;
	
	// defined in children, it must use serializeAttribute to save valuble data
	virtual void serialize()=0;
	
	virtual void logAgentState();
	virtual void updateKnowledge(){};
	virtual void selectActions(){};
	virtual void updateState(){};
	void executeActions();

	// mpi related
	virtual void sendVectorAttributes( int target ) = 0;
	virtual void receiveVectorAttributes(int origin) = 0;

	AttributesList::iterator beginStringAttributes(){ return _stringAttributes.begin(); }
	AttributesList::iterator endStringAttributes(){ return _stringAttributes.end(); }

	AttributesList::iterator beginIntAttributes(){ return _intAttributes.begin(); }
	AttributesList::iterator endIntAttributes(){ return _intAttributes.end(); }

	virtual void registerAttributes(){}
};

} // namespace Engine

#endif //__Agent_hxx__

