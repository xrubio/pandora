
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>
#include <Point2D.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{	
	int _numAgents;
	Engine::Point2D<int> _size;
public:
	RandomWorldConfig();
	virtual ~RandomWorldConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	const Engine::Point2D<int> & getSize() const;

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

