#ifndef __MDPAction_hxx__
#define __MDPAction_hxx__

#include <vector>
#include <cstdlib>
#include <iosfwd>

#include <Action.hxx>

namespace Gujarat
{
class 	GujaratAgent;
class 	HunterGathererMDPState;
class	Sector;

class MDPAction : public Engine::Action
{
	
public:
	MDPAction();
	virtual ~MDPAction();
	
	virtual void executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, HunterGathererMDPState& sp ) const;
	virtual int getTimeNeeded() const = 0;
	virtual MDPAction*	copy() const { return NULL; }
	
	virtual Sector * getVisitedSector() = 0;
	
	virtual bool equal(MDPAction* a, const GujaratAgent & gua) = 0;
	
	friend class GujaratAgent;
	
	
};
	


} // namespace Gujarat

#endif // __MDPAction_hxx__

