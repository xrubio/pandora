
#include <CustomWorld.hxx>
#include <Exception.hxx>
#include <Size.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        Test::CustomWorld world(new Engine::Config(Engine::Size<int>(64,64), 5, "sequential/data.h5"), Test::CustomWorld::useOpenMPSingleNode());
        world.initialize(argc, argv);
        world.run();
        
        Test::CustomWorld world2(new Engine::Config(Engine::Size<int>(64,64), 5, "spacePartition/data.h5"), Test::CustomWorld::useSpacePartition());
        world2.initialize(argc, argv);
        world2.run();

	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

