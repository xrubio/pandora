
#include <RasterWorld.hxx>
#include <Exception.hxx>
#include <Simulation.hxx>
#include <Size.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        Engine::Simulation sim(Engine::Size<int>(708,708), 1);
        std::string rasterFileName = "../../../resources/rasterWithColor.tiff";
        Test::RasterWorld world(sim, Test::RasterWorld::useOpenMPSingleNode("data/raster.h5"), rasterFileName);
        world.initialize(argc, argv);
        world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

