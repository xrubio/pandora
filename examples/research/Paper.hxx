
#ifndef __Paper_hxx__
#define __Paper_hxx__

#include <Agent.hxx>
#include <string>

namespace Examples
{
class Author;

class Paper : public Engine::Agent
{
	float _probabilityNewPaper; // MpiBasicAttribute
	Author * _author;
	int _citations; // MpiBasicAttribute
	int _citedPapers; // MpiBasicAttribute
	int _timeOfCreation; // MpiBasicAttribute
public:
	// todo remove environment from here
	Paper( const std::string & id, int timeOfCreation);
	virtual ~Paper();

	void updateState();
	void registerAttributes();
	void serialize();
	void generateNewPaper(); 

	void setProbabilityNewPaper( float probabilityNewPaper );
	void setAuthor( Author * author );
	Author * getAuthor();
	void incrementNumCitations();
	void setCitedPapers( int citedPapers );

	// Mpi related
	Paper( void * );
	void * fillPackage(); 
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
};

} // namespace Examples

#endif // __Paper_hxx__

