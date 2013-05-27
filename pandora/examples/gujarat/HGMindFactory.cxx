

#include <HGMindFactory.hxx>

namespace Gujarat{

void HGMindFactory::setHGMind(const std::string & type)
{
		if (type.compare("omniscient")==0)
		{
			_getHGMind = &HGMindFactory::getHGOmniMindMap;
		}
		else if (type.compare("partial")==0)
		{
			_getHGMind = &HGMindFactory::getHGPartialMindMap;
		}
		else
		{
			std::stringstream oss;
			oss << "HGMind::setHGMind - unknown type of mind: " << type;
			throw Engine::Exception(oss.str());
		}
}
	
HGMind * HGMindFactory::getHGOmniMindMap(GujaratWorld & w)
{ 
	return new HGOmniMindMap(w);
}
	
HGMind * HGMindFactory::getHGPartialMind(GujaratWorld & w)
{ 
	return new HGPartialMindMap(w);	
}
	
HGMind * HGMindFactory::getHGMind(GujaratWorld & w)
{ 
	return (this->*_getHGMind)(w);
}
	


}



