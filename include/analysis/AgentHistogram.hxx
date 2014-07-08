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

#ifndef __AgentHistogram_hxx__
#define __AgentHistogram_hxx__

#include <analysis/Output.hxx>
#include <vector>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess 
{

class AgentHistogram : public Output
{	
	std::string _attribute;
	int _interval;
	// if a num step is passed the histogram will be generated from it
	// if not, the final result of the simulation will be stored
	int _numStep;

	std::vector<int> _histogram;
public:
	AgentHistogram( const std::string & attribute, int interval = 1, int numStep = -1, const std::string & separator = ";");
	virtual ~AgentHistogram();

	void preProcess( const Engine::SimulationRecord & simRecord, const std::string & );
	void computeAgent( const Engine::AgentRecord & agentRecord );
	void postProcess( const Engine::SimulationRecord & simRecord, const std::string & );

	std::string getName() const;
};

} // namespace PostProcess

#endif // __AgentHistogram_hxx__

