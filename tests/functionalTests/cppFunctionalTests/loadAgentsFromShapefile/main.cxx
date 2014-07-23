
#include <ShpWorld.hxx>
#include <Exception.hxx>
#include <Config.hxx>
#include <Size.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        std::string shapeFileName = "../../../resources/test.shp";
        Test::ShpWorld world(new Engine::Config(Engine::Size<int>(64,64), 1), Test::ShpWorld::useOpenMPSingleNode(), shapeFileName);
        world.initialize(argc, argv);
        world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

