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

#include <Exception.hxx>
#include <SimulationRecord.hxx>

#include <analysis/GlobalAgentStats.hxx>
#include <analysis/GlobalRasterStats.hxx>
#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/RasterMean.hxx>
#include <analysis/RasterSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentStdDev.hxx>
//#include <analysis/AgentHDFtoSHP.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		Engine::SimulationRecord simRecord( 1, false);
		simRecord.loadHDF5("../simulation/data/results.h5", true, true);

		PostProcess::GlobalAgentStats agentResults;
        agentResults.addAnalysis(new PostProcess::AgentNum());
		agentResults.addAnalysis(new PostProcess::AgentMean("value"));
		agentResults.addAnalysis(new PostProcess::AgentStdDev("value"));
		agentResults.addAnalysis(new PostProcess::AgentSum("value"));

		agentResults.apply(simRecord, "agentOutput.csv", "HelloAgent");
		
        PostProcess::GlobalRasterStats rasterResults;
		rasterResults.addAnalysis(new PostProcess::RasterMean());
		rasterResults.addAnalysis(new PostProcess::RasterSum());

		rasterResults.apply(simRecord, "rasterOutput.csv", "values");
	}
	catch( Engine::Exception & exceptionThrown )
	{
		std::cout << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

