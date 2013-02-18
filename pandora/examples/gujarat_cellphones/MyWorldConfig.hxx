#ifndef __MyWorldConfig_hxx__
#define __MyWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class MyWorldConfig : public Engine::Config
{	
	int _diffVillageDaysNoCallsLow;
	int _diffVillageDaysNoCallsMedium;
	int _maxYearsCellInfo;
	int _numAgents;
	int _numAnimalsMax;
	int _numAnimalsMaxIni;
	int _numAnimalsMin;
	int _numAnimalsMinIni;
	int _numVillages;
	int _percentMapKnownAtBeginning;
	int _probabilityChildInheritsFromFather;
	int _resourcesHighLevel;
	int _resourcesLowLevel;
	int _sameVillageDaysNoCallsHigh;
	int _sameVillageDaysNoCallsMedium;
	int _size;

public:
	MyWorldConfig();
	virtual ~MyWorldConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	int getSize() const;
	int getMaxYearsCellInfo() const;
	int getNumAgents() const;
	int getNumAnimalsMax() const;
	int getNumAnimalsMaxIni() const;
	int getNumAnimalsMin() const;
	int getNumAnimalsMinIni() const;
	int getNumVillages() const;
	int getPercentMapKnownAtBeginning() const;
	int getProbabilityChildInheritsFromFather() const;
	int getResourcesHighLevel() const;
	int getResourcesLowLevel() const;
	int getSameVillageDaysNoCallsHigh() const;
	int getSameVillageDaysNoCallsMedium() const;

	friend class MyWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__
