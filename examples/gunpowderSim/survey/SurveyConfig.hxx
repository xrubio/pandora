
#ifndef __SurveyConfig_hxx__
#define __SurveyConfig_hxx__

#include <Config.hxx>

namespace BattleSim
{

class SurveyConfig: public Engine::Config
{
	std::string _battleFile;
    int _distance;
    bool _horizontal;
public:
	SurveyConfig( const std::string & xmlFile );  
	virtual ~SurveyConfig();
    
	void loadParams(); 
	friend class Survey;
	friend class Archaeologist;
};

} // namespace BattleSim 

#endif // __SurveyConfig_hxx__

