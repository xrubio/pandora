

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
	
	//static 
	GETMINDMETHOD _getHGMind;
	
	static HGMindFactory * _instance;
	
public:	
	/*static void setHGMind(const std::string & type);
	
	static HGMind *getHGOmniMindMap(GujaratWorld & w);
	
	static HGMind *getHGPartialMind(GujaratWorld & w);
	
	static HGMind *getHGMind(GujaratWorld & w);
	*/
	HGMindFactory();
	
	void setHGMind(const std::string & type);
	
	HGMind * getHGOmniMindMap(GujaratWorld & w);
	
	HGMind * getHGPartialMindMap(GujaratWorld & w);
	
	HGMind * getHGMind(GujaratWorld & w);
	
	static HGMindFactory & getInstance();
	
};

}

#endif


