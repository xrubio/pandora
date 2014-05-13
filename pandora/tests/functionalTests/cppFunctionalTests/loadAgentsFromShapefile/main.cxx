
#include <ShpWorld.hxx>
#include <Exception.hxx>
#include <Simulation.hxx>
#include <Size.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    try
    {
        std::string shapeFileName = "../../../resources/test.shp";
        Engine::Simulation sim(Engine::Size<int>(64,64), 1);
        Test::ShpWorld world(sim, Test::ShpWorld::useOpenMPSingleNode("data/shp.h5"), shapeFileName);
        world.initialize(argc, argv);
        world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

