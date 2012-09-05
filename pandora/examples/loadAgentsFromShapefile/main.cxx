
#include <ShpWorld.hxx>
#include <Exceptions.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		if(argc>2)
		{
			throw Engine::Exception("USAGE: loadAgentFromShapefile [shp file]");
		}		

		std::string shapeFileName = "resources/test.shp";
		if(argc!=1)
		{
			shapeFileName = argv[1];
		}
		Engine::Simulation sim(64, 1);
		Examples::ShpWorld world(sim, shapeFileName);
		world.init(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

