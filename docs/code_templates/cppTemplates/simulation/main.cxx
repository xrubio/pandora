
#include <HelloWorld.hxx>
#include <Exception.hxx>
#include <Config.hxx>
#include <Size.hxx>

#include <iostream>

int main(int argc, char *argv[])
{
	try
    {
        Engine::Size<int> size(10,10);
        int numSteps = 10;

		HelloWorld world(new Engine::Config(size, numSteps));
        world.initialize();
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

