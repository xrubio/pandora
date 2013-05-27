

#include <HGMindFactory.hxx>
#include <Exceptions.hxx>

namespace Gujarat{

HGMindFactory * HGMindFactory::_instance = 0;
	
HGMindFactory::HGMindFactory()
{
	// default
	_getHGMind = &HGMindFactory::getHGOmniMindMap;
}
	
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
	
HGMind * HGMindFactory::getHGPartialMindMap(GujaratWorld & w)
{ 
	return new HGPartialMindMap(w);	
}
	
HGMind * HGMindFactory::getHGMind(GujaratWorld & w)
{ 
	return (this->*_getHGMind)(w);
}
	

HGMindFactory & HGMindFactory::getInstance()
	{
		if(!_instance)
		{
			_instance = new HGMindFactory;
		}
		return *_instance;
	}
	
	
	

}



