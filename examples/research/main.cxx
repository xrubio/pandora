
#include <Academia.hxx>
#include <AcademiaConfig.hxx>
#include <Exception.hxx>

#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[])
{
	try
	{	
		if(argc>2)
		{
			throw Engine::Exception("USAGE: research [config file]");
		}		
	
		std::string fileName("config.xml");
		if(argc==2)
		{
			fileName = argv[1];
		}
		Examples::Academia academia( new Examples::AcademiaConfig(fileName), academia.useOpenMPSingleNode());
	
		academia.initialize(argc, argv);
		academia.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

