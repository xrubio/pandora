
#include <Exception.hxx>
#include <iostream>
#include "QuantumWorld.hxx"
#include "QuantumWorldConfig.hxx"
#include <Simulation.hxx>
#include <Exception.hxx>

int main(int argc, char *argv[]) {
	try {	
		if(argc>2) throw Engine::Exception("USAGE: quantumForager [config file]");	
		std::string fileName("config.xml");
		if(argc!=1) fileName = argv[1];
		QuantumExperiment::QuantumWorldConfig config;
		config.deserialize(fileName);

		Engine::Simulation myWorldSim(config.getSize(),config.getNumSteps(), config.getSerializeResolution());
		QuantumExperiment::QuantumWorld world(config, myWorldSim, world.useOpenMPSingleNode(config.getResultsFile()));

		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown ) {
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

