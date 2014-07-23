
#ifndef __RasterWorld_hxx
#define __RasterWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Config;
}

namespace Test 
{

class RasterWorld : public Engine::World
{
	void createRasters();
	void createAgents();
	std::string _rasterFile;

public:
	RasterWorld( Engine::Config * config, Engine::Scheduler * scheduler = 0, const std::string & rasterFile = "no file");
	virtual ~RasterWorld();
};

} // namespace Test 

#endif // __RasterWorld_hxx

