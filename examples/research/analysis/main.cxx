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
#include <analysis/AgentMean.hxx>
#include <analysis/AgentSum.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentHistogram.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		throw Engine::Exception("USAGE: statistics file.h5");
		return 0;		
	}

	try
	{
		Engine::SimulationRecord simRecord;
		simRecord.loadHDF5(argv[1], true, true);

		PostProcess::GlobalAgentStats paperResults;
		paperResults.addAnalysis(new PostProcess::AgentNum());
		paperResults.apply(simRecord, "papers.csv", "Paper");

		PostProcess::GlobalAgentStats authorResults;
		authorResults.addAnalysis(new PostProcess::AgentNum());
		authorResults.apply(simRecord, "authors.csv", "Author");

		PostProcess::AgentHistogram citations("citations", 3);
		PostProcess::AgentHistogram references("cited papers", 3);

		citations.apply(simRecord, "numCitationsPerPaper.csv", "Paper");
		references.apply(simRecord, "numReferencesPerPaper.csv", "Paper");
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

