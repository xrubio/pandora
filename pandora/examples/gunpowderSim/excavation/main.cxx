
#include "Site.hxx"
#include "Exceptions.hxx"

#include <iostream>
#include <cstdlib>

// this simulations tries to generate data of a XVIIIth century battlefield
// basic engagament, 0.5m each cell

int main(int argc, char *argv[])
{
	try
	{
		if(argc!=5)
		{
			throw Engine::Exception("USAGE: archaeoSim [meters between pass=3m] [horizontalMovement=1] [battlefieldFile] [resultsPath]");
		}
		// TODO arguments readed from a file
		// resolution is 0.5 meters
		int distance = (int)(2*atof(argv[1]));
		bool horizontalMovement = atoi(argv[2]);
		std::string battlefieldFile(argv[3]);
		std::string resultsPath(argv[4]);

		BattleSim::Site excavation(resultsPath, battlefieldFile, distance, horizontalMovement);
		excavation.init(argc, argv);
		excavation.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

