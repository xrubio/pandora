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

#include <SimulationRecord.hxx>

#include <analysis/Results.hxx>
#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/RasterMean.hxx>
#include <analysis/RasterSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentHDFtoSHP.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	Engine::SimulationRecord simRecord( 1, false);
	simRecord.loadHDF5("data/results.h5", true, true);

	Analysis::AgentResults agentResults(simRecord, "agents.csv", "MyAgent", ";");
	agentResults.addAnalysis(new Analysis::AgentNum());
	agentResults.addAnalysis(new Analysis::AgentMean("x"));
	agentResults.addAnalysis(new Analysis::AgentMean("y"));
	agentResults.addAnalysis(new Analysis::AgentMean("resources"));
	agentResults.addAnalysis(new Analysis::AgentSum("resources"));
	agentResults.addAnalysis(new Analysis::AgentHDFtoSHP( "shp/agents.shp'", -1));

	agentResults.apply();

	Analysis::RasterResults rasterResults(simRecord, "rasters.csv", "resources", ";");
	rasterResults.addAnalysis(new Analysis::RasterMean());
	rasterResults.addAnalysis(new Analysis::RasterSum());		

	rasterResults.apply();
	return 0;
}

