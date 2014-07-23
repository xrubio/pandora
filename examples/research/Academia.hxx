
#ifndef __Academia_hxx
#define __Academia_hxx

#include <World.hxx>

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

public:
	Academia( AcademiaConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~Academia();

	void generateNewPaper( Paper & paper );
};

} // namespace Examples 

#endif // __Academia_hxx

