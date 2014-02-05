
#include <Exceptions.hxx>
#include <iostream>

#include "MyWorld.hxx"
#include <Simulation.hxx>
#include <Point2D.hxx>

int main(int argc, char *argv[])
{
	try
	{	
		Engine::Simulation myWorldSim(Engine::Point2D<int>(64,64),10);
		Tutorial::MyWorld world(myWorldSim);
		
		world.init(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

