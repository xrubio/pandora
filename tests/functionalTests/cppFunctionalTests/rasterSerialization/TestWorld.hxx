
#ifndef __TestWorld_hxx
#define __TestWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Config;
}

namespace Test 
{

class TestWorld : public Engine::World
{
	void createRasters();
	void createAgents();
public:
	TestWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	virtual ~TestWorld();
};

} // namespace Test 

#endif // __TestWorld_hxx

