#ifndef __EarthConfig_hxx__
#define __EarthConfig_hxx__

#include <Config.hxx>
#include <Point2D.hxx>

namespace Examples
{

struct GISData
{
	int _size;
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

	int getSize() const;

	friend class Earth;
};

} // namespace Examples

#endif // __EarthConfig_hxx__

