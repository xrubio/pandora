
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Examples 
{

class RandomWorldConfig;

class RandomWorld : public Engine::World
{
	void createRasters();
	void createAgents();

	const RandomWorldConfig & _config;
public:
	RandomWorld(const RandomWorldConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0);
	virtual ~RandomWorld();
};

} // namespace Examples 

#endif // __RandomWorld_hxx

