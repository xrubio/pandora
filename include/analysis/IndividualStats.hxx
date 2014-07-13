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

#ifndef __IndividualStats_hxx__
#define __IndividualStats_hxx__

#include <analysis/Output.hxx>
#include <list>
#include <fstream>
#include <iomanip>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class IndividualStats : public Output 
{
	int _numStep;
	std::ofstream _file;

	std::list<std::string> _attributes;

	bool attributeToStore( const std::string & key );
public:
	// set a list of attributes and store for numStep time step, including agent id
	IndividualStats( int numStep, const std::string & separator=";");
	virtual ~IndividualStats();

	void computeAgent( const Engine::AgentRecord & agentRecord );
	void addAttribute( const std::string & attribute );
	void preProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );
	void postProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );

	std::string getName() const;

};

} // namespace PostProcess

#endif // __IndividualStats_hxx__

