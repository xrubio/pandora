
#ifndef __MyWorld_hxx__
#define __MyWorld_hxx__

#include <World.hxx>

namespace Engine
{
	class Config;
}

namespace Tutorial 
{

class MyWorld : public Engine::World
{
	void createRasters();
	void createAgents();

public:
	MyWorld( Engine::Config * config);
    virtual ~MyWorld();
};

} // namespace Tutorial 

#endif // __MyWorld_hxx

