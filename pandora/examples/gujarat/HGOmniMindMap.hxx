


#ifndef __HGOmniMindMap_hxx__
#define __HGOmniMindMap_hxx__

#include <HGMind.hxx>

namespace Gujarat
{
	class HunterGatherer;
	
	class HGOmniMindMap : public HGMind
	{
	public:
		HGOmniMindMap(GujaratWorld & w);
		~HGOmniMindMap();
		
		inline void updateDueToExecuteAction(Sector*) {}
		
	};
}

#endif