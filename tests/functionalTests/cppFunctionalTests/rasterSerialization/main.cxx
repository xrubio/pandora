
#include <TestWorld.hxx>
#include <Exception.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		Test::TestWorld worldSingle( new Engine::Config(Engine::Size<int>(6,4), 6*4, "single/results.h5"), Test::TestWorld::useOpenMPSingleNode());
	
		worldSingle.initialize(argc, argv);
		worldSingle.run();
		
		Test::TestWorld worldParallel( new Engine::Config(Engine::Size<int>(6,4), 6*4, "parallel/results.h5"), Test::TestWorld::useSpacePartition());
	
		worldParallel.initialize(argc, argv);
		worldParallel.run();

	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

