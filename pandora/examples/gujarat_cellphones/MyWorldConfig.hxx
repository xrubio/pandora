#ifndef __MyWorldConfig_hxx__
#define __MyWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class MyWorldConfig : public Engine::Config
{	
	int _numAgents;
	int _numAnimalsMax;
	int _numAnimalsMaxIni;
	int _numAnimalsMin;
	int _numAnimalsMinIni;
	int _numVillages;
	int _probabilityChildInheritsFromFather;
	int _size;

public:
	MyWorldConfig();
	virtual ~MyWorldConfig();

	void extractParticularAttribs(TiXmlElement *pRoot);

	int getSize() const;
	int getNumAgents() const;
	int getNumAnimalsMax() const;
	int getNumAnimalsMaxIni() const;
	int getNumAnimalsMin() const;
	int getNumAnimalsMinIni() const;
	int getProbabilityChildInheritsFromFather() const;
	int getNumVillages() const;

	friend class MyWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__
