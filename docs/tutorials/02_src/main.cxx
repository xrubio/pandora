
#include <Exceptions.hxx>
#include <iostream>

#include "MyWorld.hxx"
#include <Simulation.hxx>

int main(int argc, char *argv[])
{
	try
	{	
		Engine::Simulation myWorldSim(64,10);
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

