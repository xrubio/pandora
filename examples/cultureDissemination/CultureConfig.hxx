
#ifndef __CultureConfig_hxx__
#define __CultureConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class CultureConfig : public Engine::Config
{	
    // number of features per agent
    int _features;
    // possible traits for each feature
    int _traitsPerFeature;
    // probability of transmission
    float _transmission;
    // range of transmission
    float _range;
    // if true distance between traits will be computed
    // if false distance is always 1 unless they are identical
    bool _useDistance;
public:
	CultureConfig( const std::string & xmlFile );
	virtual ~CultureConfig();

	void loadParams();

	friend class CultureWorld;
	friend class Site;
};

} // namespace Examples

#endif // __CultureConfig_hxx__

