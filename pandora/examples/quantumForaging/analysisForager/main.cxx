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

#include <analysis/Results.hxx>
#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentFinalResults.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc!=3)
	{
		throw Engine::Exception("USAGE: analysis file.h5 results.csv");
		return 0;
	}

	try
	{
		Engine::SimulationRecord simRecord( 1, false);
		simRecord.loadHDF5(argv[1], false, true);

		Analysis::AgentResults agentResults(simRecord, "", "Forager");
		
		Analysis::AgentFinalResults * final = new Analysis::AgentFinalResults(argv[2], simRecord.getNumSteps()/simRecord.getSerializedResolution(), ";");
		final->addAttribute("needed resources");
		final->addAttribute("starvation x100");
		final->addAttribute("width");
		final->addAttribute("horizon");
		final->addAttribute("move actions");
		final->addAttribute("risk aversion x100");
		agentResults.addAnalysis(final);

		agentResults.apply();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

