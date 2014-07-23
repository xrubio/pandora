#ifndef __HunterGathererMDPConfig_hxx__
#define __HunterGathererMDPConfig_hxx__

namespace Gujarat
{
class GujaratConfig;

class HunterGathererMDPConfig
{
public:
	HunterGathererMDPConfig();
	virtual ~HunterGathererMDPConfig();	


	int	getNumberForageActions() const { return _nrForageActions; }
	int	getNumberMoveHomeActions() const { return _nrMoveHomeActions; }
	bool	isDoNothingAllowed() const { return _doNothingAllowed; } 
	int	getHorizon() const { return _horizon; } 
	int	getWidth() const { return _width; }
	float	getExplorationBonus() const { return _explorationBonus; }

private:

	int	_nrForageActions;
	int	_nrMoveHomeActions;
	bool	_doNothingAllowed;
	int	_horizon;
	int	_width;
	float	_explorationBonus;

public:
    friend class GujaratConfig;
};

}

#endif // HunterGathererMDPConfig.hxx

