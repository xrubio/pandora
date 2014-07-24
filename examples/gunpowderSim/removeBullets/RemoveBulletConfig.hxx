
#ifndef __RemoveBulletConfig_hxx__
#define __RemoveBulletConfig_hxx__

#include <Config.hxx>

namespace BattleSim
{

class RemoveBulletConfig: public Engine::Config
{
	std::string _battleFile;
	float _lossProbability;
public:
	RemoveBulletConfig( const std::string & xmlFile );  
	virtual ~RemoveBulletConfig();
    
	void loadParams(); 
	friend class RemoveBulletWorld;
};

} // namespace BattleSim 

#endif // __RemoveBulletConfig_hxx__

