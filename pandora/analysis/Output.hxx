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

#ifndef __Output_hxx__
#define __Output_hxx__

#include <list>
#include <string>

namespace Engine
{
	class SimulationRecord;
	class AgentRecord;
}

namespace PostProcess
{

class Output
{
protected:
	std::string _separator;

	virtual void preProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );
	virtual void postProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );
	virtual void computeAgent( const Engine::AgentRecord & agentRecord );

public:
	Output( const std::string & separator );
	virtual ~Output();

	virtual void apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type );
	virtual std::string getName() const = 0;
};

} // namespace PostProcess

#endif // __Output_hxx__

