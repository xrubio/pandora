
#include <TestWorld.hxx>
#include <Exceptions.hxx>
#include <Simulation.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		Engine::Simulation sim(Engine::Size<int>(6,4), 6*4, 1);
		Test::TestWorld worldSingle( sim, Test::TestWorld::useOpenMPSingleNode("single/results.h5"));
	
		worldSingle.initialize(argc, argv);
		worldSingle.run();
		
		Test::TestWorld worldParallel( sim, Test::TestWorld::useSpacePartition("parallel/results.h5"));
	
		worldParallel.initialize(argc, argv);
		worldParallel.run();

	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

