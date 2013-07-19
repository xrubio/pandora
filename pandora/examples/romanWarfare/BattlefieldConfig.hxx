
#ifndef __BattlefieldConfig_hxx__
#define __BattlefieldConfig_hxx__

#include <Config.hxx>
#include <string>
#include <list>

class TiXmlElement;

namespace ClassicalWarfare 
{
class Battlefield;

struct Line
{
	int _numBlocks;
	int _ranks;
	int _columns;
	int _separationBetweenBlocks;
	int _offset;
};

class BattlefieldConfig : public Engine::Config
{
	void extractParticularAttribs(TiXmlElement * root);

	void loadArmy(TiXmlElement * element, bool isRed);
public:
	int _size;
	// distance between opponent battle lines at the beginning of the simulation
	int _initialDistance;

	int _redSeparationBetweenLines;
	std::list<Line> _redLines;

	int _blueSeparationBetweenLines;
	std::list<Line> _blueLines;

public:
	BattlefieldConfig();
	virtual ~BattlefieldConfig();

	friend class Battlefield;
};

} // namespace ClassicalWarfare

#endif // __BattlefieldConfig_hxx__

