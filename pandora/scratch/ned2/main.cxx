
#include "NEDConfig.hxx"
#include "NEDWorld.hxx"
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
			oss << "USAGE: ned "<< std::endl <<"and num. args:: "<< argc;
			throw Engine::Exception(oss.str());
		}		
		
		std::string fileName("config.xml");
		if(argc!=1)
		{
		  fileName = argv[1];
		}
		NEDConfig c;
		c.deserialize(fileName);
		
		std::cout << "config> " << c << std::endl;		
		
		Engine::Simulation NEDsim(c.getSize(),c.getNumSteps()); // TODO pass the Config object c
		NED::NEDWorld world(NEDsim, c);
		
		world.init(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

