#ifndef __EarthConfig_hxx__
#define __EarthConfig_hxx__

#include <Config.hxx>
#include <Point2D.hxx>

namespace Examples
{

struct GISData
{
	Engine::Point2D<int> _size;
	int _resolution;
	std::string _demName;
	std::string _populationName;
};

class EarthConfig : public Engine::Config
{
	GISData _gisData;
	// number of real individuals to 1 in simulation
	int _scale;
	Engine::Point2D<int> _firstCase;
	int _numCases;
public:
	EarthConfig();
	virtual ~EarthConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	const Engine::Point2D<int> & getSize() const;

	friend class Earth;
};

} // namespace Examples

#endif // __EarthConfig_hxx__

