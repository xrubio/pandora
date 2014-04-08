
#include "TestWorld.hxx"
#include <Exceptions.hxx>
#include <Simulation.hxx>

#include <iostream>
#include <cstdlib>

// this test checks the behavior of Pandora for large numbers of rasters 

int main(int argc, char *argv[])
{
	try
	{
		if(argc>3)
		{
			throw Engine::Exception("USAGE: testMultipleRasters [number of rasters] [time steps]");
		}
		int numRasters = 1;
		int numTimeSteps = 1;
		if(argc==3)
		{
			numTimeSteps = atoi(argv[2]);
			numRasters = atoi(argv[1]);
		}
		else if(argc==2)
		{
			numRasters = atoi(argv[1]);
		}
		Engine::Simulation sim(1600, numTimeSteps);
		Test::TestWorld multipleRasters(sim,numRasters);
		
		multipleRasters.init(argc, argv);
		multipleRasters.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

