
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

#ifndef __Analysis_hxx__
#define __Analysis_hxx__

#include <string>
#include <vector>

#include <SimulationRecord.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess 
{

class Analysis
{
protected:
	std::string _name;
	std::vector<long double> _results;
	// if _writeResults is false _results will not be written in csv file
	// it is used for analysis that produce other results (i.e. AgentHDFtoSHP parser)
	bool _writeResults;
public:
	Analysis( const std::string & name, bool writeResults = true );
	virtual ~Analysis();
	void setNumTimeSteps( int numTimeSteps );

	const std::string & getName() const;
	virtual void preProcess();
	virtual void postProcess(){};
	long double getResult( int timeStep ) const;
	bool writeResults(){return _writeResults;}	
};

class RasterAnalysis : public Analysis
{
public:
	RasterAnalysis( const std::string & name, bool writeResults = true ) : Analysis(name, writeResults)
	{
	}
	virtual ~RasterAnalysis(){}
	virtual void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory ) = 0;
};

class AgentAnalysis : public Analysis
{
public:
	AgentAnalysis( const std::string & name, bool writeResults = true ) : Analysis(name, writeResults)
	{
	}
	virtual ~AgentAnalysis(){}
	virtual void computeAgent( const Engine::AgentRecord & ) = 0;
};

} // namespace PostProcess

#endif // __Analysis_hxx__
