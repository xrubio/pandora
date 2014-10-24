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

#include <analysis/GlobalAgentStats.hxx>
#include <analysis/GlobalRasterStats.hxx>
#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/RasterMean.hxx>
#include <analysis/RasterSum.hxx>
#include <analysis/AgentNum.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	Engine::SimulationRecord simRecord( 1, false);
	simRecord.loadHDF5("data/results.h5", true, true);

	PostProcess::GlobalAgentStats agentResults;	
	agentResults.addAnalysis(std::shared_ptr<PostProcess::AgentNum> (new PostProcess::AgentNum()));
	agentResults.addAnalysis(std::shared_ptr<PostProcess::AgentMean> (new PostProcess::AgentMean("x")));
	agentResults.addAnalysis(std::shared_ptr<PostProcess::AgentMean> (new PostProcess::AgentMean("y")));
	agentResults.addAnalysis(std::shared_ptr<PostProcess::AgentMean> (new PostProcess::AgentMean("resources")));
	agentResults.addAnalysis(std::shared_ptr<PostProcess::AgentSum> (new PostProcess::AgentSum("resources")));

	agentResults.apply(simRecord, "agents.csv", "MyAgent");

	PostProcess::GlobalRasterStats rasterResults;
	rasterResults.addAnalysis(std::shared_ptr<PostProcess::RasterMean> (new PostProcess::RasterMean()));
	rasterResults.addAnalysis(std::shared_ptr<PostProcess::RasterSum> (new PostProcess::RasterSum()));
	rasterResults.apply(simRecord, "rasters.csv", "resources");
	return 0;
}

