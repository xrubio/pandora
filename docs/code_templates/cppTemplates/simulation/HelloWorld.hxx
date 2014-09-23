
#ifndef __HelloWorld_hxx__
#define __HelloWorld_hxx__

#include <World.hxx>

namespace Engine
{
	class Config;
}

class HelloWorld : public Engine::World
{
	void createRasters();
	void createAgents();
    void stepEnvironment();

public:
	HelloWorld(Engine::Config * config);
	virtual ~HelloWorld();
};

#endif // __HelloWorld_hxx__

