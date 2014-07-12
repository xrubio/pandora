
#ifndef __Academia_hxx
#define __Academia_hxx

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Examples 
{

class AcademiaConfig;
class Paper;

class Academia : public Engine::World
{
	long int _newPaperKey;
	long int _newAuthorKey;

	void createRasters();
	void createAgents();

	const AcademiaConfig & _config;
public:
	Academia( const AcademiaConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0);
	virtual ~Academia();

	void generateNewPaper( Paper & paper );
};

} // namespace Examples 

#endif // __Academia_hxx

