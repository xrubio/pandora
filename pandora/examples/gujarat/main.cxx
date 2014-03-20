
#include <GujaratWorld.hxx>
#include <GujaratConfig.hxx>
#include <Simulation.hxx>
#include <Exceptions.hxx>
#include <iostream>
#include <cstdlib>

#include <stdlib.h>

#include <omp.h>

//#include <mcheck.h>
int main(int argc, char *argv[])
{
//mtrace();
	try
	{
		if(argc>2)
		{
			throw Engine::Exception("USAGE: gujarat [config file]");
		}		

		std::string fileName("config.xml");
		if(argc!=1)
		{
			fileName = argv[1];
		}

		omp_set_num_threads(4);


// Should wire the same CPU to the same threads 
//export OMP_NUM_THREADS=4
//export OMP_WAIT_POLICY=ACTIVE
//export GOMP_CPU_AFFINITY="0-8:2"

		Gujarat::GujaratConfig config;
		std::cout << "Loading config file: " << fileName << std::endl;
		config.deserialize(fileName);

		srand48(config.getUCTSeed());
		
		std::cout << "main: UCT SEED " << config.getUCTSeed() << std::endl;
		
		Engine::Simulation gujaratSim(config.getSize(), config.getNumSteps(), config.getSerializeResolution());
		Gujarat::GujaratWorld world( gujaratSim , config);
	
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

