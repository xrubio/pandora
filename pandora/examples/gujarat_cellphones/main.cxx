#include <Exceptions.hxx>
#include <iostream>
#include "MyWorld.hxx"
#include "MyWorldConfig.hxx"
#include <Simulation.hxx>
#include <Exceptions.hxx>

int main(int argc, char *argv[]) {
	try {	
		if(argc>2) throw Engine::Exception("USAGE: randomTest [config file]");	
		std::string fileName("config.xml");
		if(argc!=1) fileName = argv[1];
		GujaratCellphones::MyWorldConfig config;
		config.deserialize(fileName);

		Engine::Simulation myWorldSim(config.getSize(),config.getNumSteps());
		GujaratCellphones::MyWorld world(myWorldSim, config);

		world.init(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown ) {
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

