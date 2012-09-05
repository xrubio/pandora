
#include "SugarConfig.hxx"
#include "SugarWorld.hxx"
#include "Simulation.hxx"
#include "Exceptions.hxx"

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{
		if(argc>2)
		{
			std::stringstream oss;
			oss << "USAGE: sugar "<< std::endl <<"and num. args:: "<< argc;
			throw Engine::Exception(oss.str());
		}		
		
		std::string fileName("config.xml");
		if(argc!=1)
		{
		  fileName = argv[1];
		}
		SUG::SugarConfig c;
		// c.deserialize(fileName);
		
		std::cout << "config> " << c << std::endl;		
		
		Engine::Simulation SUGsim(c.getSize(),c.getNumSteps()); // TODO pass the Config object c
		std::cout << "creating World object" << std::endl;
		SUG::SugarWorld world(SUGsim, c);
		std::cout << "created World object" << std::endl;
		world.init(argc, argv);
		std::cout << "RUN" << std::endl;
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

