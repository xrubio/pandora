#ifndef __HunterGathererMDPConfig_hxx__
#define __HunterGathererMDPConfig_hxx__

#include <iosfwd>

class TiXmlElement;

namespace Gujarat
{

class HunterGathererMDPConfig
{
public:
	HunterGathererMDPConfig();
	HunterGathererMDPConfig( TiXmlElement* elem );
	~HunterGathererMDPConfig();	


	int	getNumberForageActions() const { return _nrForageActions; }
	int	getNumberMoveHomeActions() const { return _nrMoveHomeActions; }
	bool	isDoNothingAllowed() const { return _doNothingAllowed; } 
	int	getHorizon() const { return _horizon; } 
	int	getWidth() const { return _width; }
	float	getExplorationBonus() const { return _explorationBonus; }

	void	dump( std::ostream& os ) const;

private:

	int	_nrForageActions;
	int	_nrMoveHomeActions;
	bool	_doNothingAllowed;
	int	_horizon;
	int	_width;
	float	_explorationBonus;

};

}

#endif // HunterGathererMDPConfig.hxx
