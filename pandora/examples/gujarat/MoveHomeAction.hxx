
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
	Engine::Point2D<int>	_newHomeLocLR;
	// forage with half adult population is executed at the same time than MoveHome
	ForageAction * _forageAction;     
public:
	MoveHomeAction( const Engine::Point2D<int>& p, const Engine::Point2D<int>& pLR, Sector * HRSectorToForage, Sector * LRSectorToForage, bool ownsSectorPointer = false );
	
	// constructor copying forage action
	MoveHomeAction( const Engine::Point2D<int>& p, const Engine::Point2D<int>& pLR, ForageAction * forageAction );
	virtual ~MoveHomeAction();
	// TODO templatitzar classe per agent
	void execute( Engine::Agent & agent );
	
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;

	int getTimeNeeded() const;

	static void	generatePossibleActions( const GujaratAgent& agent
										, const std::vector< Sector* > & HRActionSectors
										, const std::vector< Sector* > & LRActionSectors
										, std::vector< MoveHomeAction* >& actions);
	
	static void	generatePossibleActions( const GujaratAgent& agent
										, const Engine::Point2D<int>& agentPos
										, const std::vector< Sector* > & HRActionSectors
										, const std::vector< Sector* > & LRActionSectors
										, std::vector< MoveHomeAction* > & actions);

	MDPAction* copy() const;
	std::string describe() const;
	int getPotentialBiomass() const;
	
	Sector * getVisitedSector();
	/*Sector * getVisitedSector(GujaratAgent & ag) 
	 * { return ag.getSectorOfLRCell(_newHomeLoc); }
	 * */
	
	Engine::Point2D<int> getNewHomeLoc() const { return _newHomeLoc; }
	Engine::Point2D<int> getNewHomeLocLR() const { return _newHomeLocLR; }
	
	
#ifdef REDUCC	
	bool equal(MDPAction * a, const GujaratAgent & gua) { return 		
							dynamic_cast<MoveHomeAction*>(a) 
							//&& _newHomeLoc == ((MoveHomeAction*)a)->_newHomeLoc
							&& _newHomeLocLR == ((MoveHomeAction*)a)->_newHomeLocLR 
							&& ((MDPAction*)_forageAction)->equal(
									(MDPAction*)((MoveHomeAction*)a)->_forageAction
									,gua
								); }
#endif
	
	
#ifndef REDUCC		
	bool equal(MDPAction * a, const GujaratAgent & gua) { return 		
							dynamic_cast<MoveHomeAction*>(a) 
							&& _newHomeLoc == ((MoveHomeAction*)a)->_newHomeLoc
							&& ((MDPAction*)_forageAction)->equal(
									(MDPAction*)((MoveHomeAction*)a)->_forageAction
									,gua
								); }
#endif
	
	
};
	


} // namespace Gujarat

#endif // __MoveHomeAction_hxx__

