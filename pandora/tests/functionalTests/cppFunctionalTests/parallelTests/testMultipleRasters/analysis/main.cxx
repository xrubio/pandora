#include <Exceptions.hxx>
#include <Simulation.hxx>

#include <SimulationRecord.hxx>
#include <analysis/Results.hxx>
#include <analysis/RasterMean.hxx>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{
		if(argc!=2)
		{
			throw Engine::Exception("USAGE: testMultipleRasters [number of rasters]");
		}
		int numRasters = atoi(argv[1]);
	
		Engine::SimulationRecord simRecord( 1, false);
		simRecord.loadHDF5("../data/test.h5", true, false);
		for(int i=0; i<numRasters; i++)
		{
			std::stringstream logName;
			logName << "raster_" << i;
			Analysis::RasterResults rasterResults(simRecord, logName.str()+".csv", logName.str());
			rasterResults.addAnalysis(new Analysis::RasterMean());
			rasterResults.apply();
		}
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

