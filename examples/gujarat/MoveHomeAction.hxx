
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <MDPAction.hxx>

#include <Point2D.hxx>
#include <vector>

namespace Engine
{
	class Agent;
}

namespace Gujarat
{

class GujaratAgent;
class Sector;
class ForageAction;
	
class MoveHomeAction : public MDPAction
{
	Engine::Point2D<int>	_newHomeLoc;
	// forage with half adult population is executed at the same time than MoveHome
	ForageAction * _forageAction;     
public:
	MoveHomeAction( const Engine::Point2D<int>& p, Sector * sectorToForage, bool ownsSectorPointer = false );
	// constructor copying forage action
	MoveHomeAction( const Engine::Point2D<int>& p, ForageAction * forageAction );
	virtual ~MoveHomeAction();
	// TODO templatitzar classe per agent
	void execute( Engine::Agent & agent );
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;

	int getTimeNeeded() const;

	static void	generatePossibleActions( const GujaratAgent& agent, std::vector< MoveHomeAction* >& actions );
	static void	generatePossibleActions( const GujaratAgent& agent, const Engine::Point2D<int>& agentPos, std::vector< MoveHomeAction* >& actions );

	MDPAction* copy() const;
	std::string describe() const;
	int getPotentialBiomass() const;
};
	
} // namespace Gujarat

#endif // __MoveHomeAction_hxx__

