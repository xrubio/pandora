

#ifndef __HGMindFactory_hxx__
#define __HGMindFactory_hxx__


#include <string>
#include <HGMind.hxx>
#include <HGOmniMindMap.hxx>
#include <HGPartialMindMap.hxx>
#include <GujaratWorld.hxx>

namespace Gujarat{

class HGMindFactory
{
	//class GujaratWorld;
	
	typedef HGMind* (HGMindFactory::*GETMINDMETHOD) (GujaratWorld & w);
	
	static GETMINDMETHOD _getHGMind;
public:	
	static void setHGMind(const std::string & type);
	
	static HGMind *getHGOmniMindMap(GujaratWorld & w);
	
	static HGMind *getHGPartialMind(GujaratWorld & w);
	
	static HGMind *getHGMind(GujaratWorld & w);
	
};

}

#endif


