
#ifndef __ShpWorld_hxx
#define __ShpWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Config;
}

namespace Test 
{

class ShpWorld : public Engine::World
{
	void createRasters();
	void createAgents();
	std::string _shpFile;

public:
	ShpWorld( Engine::Config * config, Engine::Scheduler * scheduler = 0, const std::string & shpFile = "no file");
	virtual ~ShpWorld();
};

} // namespace Test 

#endif // __ShpWorld_hxx

