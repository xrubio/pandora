
#ifndef __MyWorld_hxx__
#define __MyWorld_hxx__

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Tutorial 
{

class MyWorldConfig;

class MyWorld : public Engine::World
{
	void createRasters();
	void createAgents();

public:
	MyWorld( Engine::Simulation & simulation );
	virtual ~MyWorld();
};

} // namespace Tutorial 

#endif // __MyWorld_hxx

