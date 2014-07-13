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

#include "Exception.hxx"
#include "SimulationRecord.hxx"

#include "analysis/Results.hxx"
#include "analysis/AgentMean.hxx"
#include "analysis/AgentSum.hxx"
#include "analysis/RasterMean.hxx"
#include "analysis/RasterSum.hxx"
#include "analysis/AgentNum.hxx"
#include <analysis/AgentHDFtoSHP.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		Engine::SimulationRecord simRecord;
		simRecord.loadHDF5("../data/epidemy.h5", true, true);

		Analysis::AgentResults humanData(simRecord, "humans.csv", "Human");
		humanData.addAnalysis(new Analysis::AgentNum());
		
		Analysis::AgentResults zombieData(simRecord, "zombies.csv", "Zombie");
		zombieData.addAnalysis(new Analysis::AgentNum());
		zombieData.addAnalysis(new Analysis::AgentHDFtoSHP("zombies.shp", Engine::Point2D<int>(257995, 4752020), 1000.0f, "EPSG:23031"));

		humanData.apply();
		zombieData.apply();
		
		Analysis::RasterResults rasterHumanData(simRecord, "rasterHumans.csv", "humans");
		rasterHumanData.addAnalysis(new Analysis::RasterMean());
		rasterHumanData.addAnalysis(new Analysis::RasterSum());	
		
		Analysis::RasterResults rasterZombieData(simRecord, "rasterZombies.csv", "zombies");
		rasterZombieData.addAnalysis(new Analysis::RasterMean());
		rasterZombieData.addAnalysis(new Analysis::RasterSum());		

		rasterHumanData.apply();
		rasterZombieData.apply();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

