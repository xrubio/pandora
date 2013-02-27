
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{	
	int _numAgents;
	int _size;

	// mdp
	int _horizon;
	int _width;
	int _explorationBonus;

	int _neededResources;
public:
	RandomWorldConfig();
	virtual ~RandomWorldConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	int getSize() const;

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

