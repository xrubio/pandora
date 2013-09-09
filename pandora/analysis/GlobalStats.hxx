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

#ifndef __GlobalStats_hxx__
#define __GlobalStats_hxx__

#include <analysis/Output.hxx>

namespace PostProcess
{
class AgentAnalysis;

class GlobalStats : public Output
{
public:
	typedef std::list< std::list<std::string> > Params;
protected:
	typedef std::list<AgentAnalysis *> AgentAnalysisList;
	AgentAnalysisList _analysisList;

	// if true analysis will be deleted in destructor
	bool _analysisOwnership;

	Params * _params;
	std::string _groupFile;

public:
	GlobalStats( const std::string & separator=";");	
	virtual ~GlobalStats();

	void setAnalysisOwnership( bool analysisOwnership );
	void apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type );
	void addAnalysis( AgentAnalysis * analysis );

	std::string getName() const;
	void setParams( Params * params, const std::string & groupFile);
};

} // namespace PostProcess

#endif // __GlobalStats_hxx__

