
#ifndef __Author_hxx__
#define __Author_hxx__

#include <Agent.hxx>

namespace Examples
{

class Author : public Engine::Agent
{
	int _researchLife; // MpiBasicAttribute;
	int _researchLifeMaximum; // MpiBasicAttribute
	int _numPapers; // MpiBasicAttribute
	int _numCitations; // MpiBasicAttribute
public:
	Author( const std::string & id, int researchLifeMaximum );
	virtual ~Author();
	void updateState();
	void registerAttributes();
	void serialize();

	bool isActive() const;
	void incrementNumPapers();
	void incrementNumCitations();

	// Mpi related
	Author( void * );
	void * fillPackage(); 
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
};

} // namespace Examples

#endif // __Author_hxx__

