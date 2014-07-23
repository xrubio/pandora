
#include <RasterWorld.hxx>
#include <Exception.hxx>
#include <Config.hxx>
#include <Size.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        std::string rasterFileName = "../../../resources/test.tiff";
        Test::RasterWorld world(new Engine::Config(Engine::Size<int>(120,120), 1), Test::RasterWorld::useOpenMPSingleNode(), rasterFileName);
        world.initialize(argc, argv);
        world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

