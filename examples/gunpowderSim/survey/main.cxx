
#include <Exception.hxx>
#include "Survey.hxx"
#include "SurveyConfig.hxx"

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{        
        if(argc>2)
		{
			throw Engine::Exception("USAGE: survey [config file]");
		}
		
		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}
		BattleSim::Survey survey( new BattleSim::SurveyConfig(fileName), survey.useOpenMPSingleNode());
		survey.initialize(argc, argv);
		survey.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

