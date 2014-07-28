
#include "TestWorld.hxx"
#include <Exception.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>

// this test checks the behavior of Pandora for large numbers of rasters 

int main(int argc, char *argv[])
{
	try
	{
		Test::TestWorld world(new Engine::Config(Engine::Size<int>(1600,1600), 10), world.useSpacePartition(199));
		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

