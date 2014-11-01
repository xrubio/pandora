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

#ifndef __GlobalRasterStats_hxx__
#define __GlobalRasterStats_hxx__

#include <analysis/Output.hxx>
#include <memory>

namespace PostProcess
{
class RasterAnalysis;

class GlobalRasterStats : public Output
{
public:
	typedef std::list< std::list<std::string> > Params;
protected:
	typedef std::list<std::shared_ptr<RasterAnalysis> > RasterAnalysisList;
	RasterAnalysisList _analysisList;

	// if true analysis will be deleted in destructor
	bool _analysisOwnership;

	Params * _params;
	std::string _groupFile;
	std::string _inputDir;

	void writeParams( std::stringstream & line, const std::string & fileName );
public:
	GlobalRasterStats( const std::string & separator=";");	
	virtual ~GlobalRasterStats();

	void setAnalysisOwnership( bool analysisOwnership );
	void apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type );
	void addAnalysis( RasterAnalysis * analysis );
	void addAnalysis( std::shared_ptr<RasterAnalysis> analysis );

	std::string getName() const;
	// set params for performing group analysis. If a single analysis is applied a time series file will be generated following resolution and numSteps.
	void setParams( Params * params, const std::string & groupFile, const std::string & inputDir, int numSteps=1, int resolution=1);
};

} // namespace PostProcess

#endif // __GlobalRasterStats_hxx__

