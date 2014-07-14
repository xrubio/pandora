
#ifndef __Author_hxx__
#define __Author_hxx__

#include <Agent.hxx>

namespace Examples
{

class Paper;

class Author : public Engine::Agent
{
	int _researchLife; // MpiBasicAttribute;
	int _researchLifeMaximum; // MpiBasicAttribute
	int _numPapers; // MpiBasicAttribute
	int _numCitations; // MpiBasicAttribute

    std::list<Engine::Point2D<int> > _paperPositions;
    
public:
	Author( const std::string & id, int researchLifeMaximum );
	virtual ~Author();
	void updateState();
	void registerAttributes();
	void serialize();

	bool isActive() const;
	void incrementNumPapers( const Engine::Point2D<int> & paperPosition );
	void incrementNumCitations();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Author( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __Author_hxx__

