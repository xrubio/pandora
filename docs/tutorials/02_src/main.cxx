
#include <Exception.hxx>
#include <iostream>

#include "MyWorld.hxx"
#include <Config.hxx>
#include <Point2D.hxx>

int main(int argc, char *argv[])
{
	try
	{	
		Tutorial::MyWorld world(new Engine::Config(Engine::Size<int>(64,64),10));
		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

