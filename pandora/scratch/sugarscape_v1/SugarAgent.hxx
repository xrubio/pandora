
#ifndef __SugarAgent_hxx__
#define __SugarAgent_hxx__

#include "Agent.hxx"
#include "SugarWorld.hxx"
#include <string>
#include "Point2D.hxx"

#include "SugarConfig.hxx"

namespace SUG
{

#define	NUMNEIGHBOURS 4
	
enum childBearing
{ 
	CHILDHOOD, 
	PUBERTY, 
	MATURE, 
	PAUSIA 
};

enum SexCategories
{
	MALE,
	FEMALE
};

enum SugarAgentState
{
	ALIVE,
	DEAD
};

typedef struct
{
	char _id[32];
	Engine::Point2D<int> _position;	
	bool _exists;
	int  _sugar;
	int  _initialEndowment;	
	SugarAgentState _state;
	SexCategories _sex;
	int  _age;
	int  _beginChildBearingAge;
	int  _beginPausiaAge;
	
	// DNA
	int _vision;
	int _deathAge;
	int _metabolism;
} SugarAgentPackage;

class SugarAgent : public Engine::Agent
{
/*
	static int _cultureSize;
	static int _immuneSystemSize;
	static int _metabolismSize;
	static int _resourceAccumulationsSize;
*/
	SugarWorld * _world;

	protected:
	int 					_sugar;
	int						_initialEndowment;
	bool					_exists;
	SugarAgentState			_state;
	SexCategories			_sex;
	int						_age;
	int						_beginChildBearingAge;
	int						_beginPausiaAge;
	// DNA
	int						_vision;
	int						_deathAge;
	int						_metabolism;

    static SugarConfig _config;

	// methods
	//Engine::Point2D<int> positionOffspring();

public:
	SugarAgent( const std::string & id );
	SugarAgent( const std::string & id, SugarWorld * w, Engine::Point2D<int> position); 
	SugarAgent( const SugarAgentPackage & package );
	SugarAgent( const std::string & id 
				, Engine::Point2D<int> position
				, bool exists
				, SugarAgentState state
				, SugarWorld* world
				, int sugar
				, int initialEndowment
				, int vision
				, int age
				, int maxAge
				, int metabolism
				, int beginChildBearingAge
				, int beginPausiaAge
				, SexCategories sex ); 
	
	virtual ~SugarAgent();
	
	void step();
	void move();
	void metabolism();
	void setState(SugarAgentState s);	
	SugarAgent* mating(SugarAgent &a,Engine::Point2D<int> & bornPosition);
	bool isFertile() { return _age    <  _deathAge 
							&& _state != DEAD 
							&& _age   >= _beginChildBearingAge 
							&& _age   <  _beginPausiaAge 
							&& _sugar >= _initialEndowment; }
	
	void crossOver(SugarAgent * x, SugarAgent * y);
	void mating();
	SugarAgent* matingWithPartner(SugarAgent * a,Engine::Point2D<int> & bornPosition);
	void death();
	void eat();	
	bool starvation();
	bool checkDeath();
	
	Engine::Point2D<int> allocateNewBorn(SugarAgent * b);
	//void producePollution();
	void sexualReproduction();
	//void inheritance();	
	
	void checkMoveCell(Engine::Point2D<int> & newPosition
					, int *maxValue
					, int *bestDistance
					, int distance
					, std::vector< Engine::Point2D<int> > * possiblePositions);	
	
	friend std::ostream & operator<<( std::ostream & stream, SugarAgent & a )
	{
		return stream <<	"Agent<pos "	<< a._position	<<	
							", sugar "		<< a._sugar		<<
							", initialEndowment" << a._initialEndowment	<<
							", exists "		<< a._exists		<<
							", sex "		<< a._sex			<<
							", age "		<< a._age			<<
							", vision "		<< a._vision		<<
							", death "		<< a._deathAge	<<
							", metab "		<< a._metabolism << ">";		
	}
	/*
	friend std::ostream & operator<<( std::ostream & stream, sugarAgent * agent );
	*/
	void * createPackage();
	void serialize();
	
	void setHomeRange( const int & homeRange );
	void setSplitChance( const int & spliChance );
	void setCannibalism( const int & cannibalism );
	void setResources( const int & resources );	
	
	static void setConfig(SugarConfig c);
};

} // namespace SUG

#endif // __SugarAgent_hxx__

