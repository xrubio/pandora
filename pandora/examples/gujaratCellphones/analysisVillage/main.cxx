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

#include <Exceptions.hxx>
#include <SimulationRecord.hxx>

#include <analysis/GlobalAgentStats.hxx>
#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/IndividualStats.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc!=4)
	{
		throw Engine::Exception("USAGE: analysis file.h5 globalStats.csv individualStats.csv");
		return 0;
	}

	try
	{
		Engine::SimulationRecord simRecord( 1, false);
		simRecord.loadHDF5(argv[1], false, true);

		PostProcess::GlobalAgentStats agentResults;
		
		agentResults.addAnalysis(new PostProcess::AgentNum());
//		agentResults.addAnalysis(new PostProcess::AgentMean("starvation x100"));
//		agentResults.addAnalysis(new PostProcess::AgentMean("herd size"));
//		agentResults.addAnalysis(new PostProcess::AgentMean("needed resources"));

		PostProcess::IndividualStats stats(simRecord.getNumSteps()/simRecord.getSerializedResolution());
		stats.addAttribute("in village transmission");
		stats.addAttribute("herders");
		stats.addAttribute("total animals");
		stats.addAttribute("known 3-year cells");
		
		agentResults.apply(simRecord, argv[2], "Village");		
		stats.apply(simRecord, argv[3], "Village");
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

