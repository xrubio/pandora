
#ifndef __CultivatedField_hxx__
#define __CultivatedField_hxx__

#include <Point2D.hxx>

namespace Gujarat
{
class GujaratWorld;

class CultivatedField
{
	Engine::Point2D<int> _position;
	GujaratWorld & _world;
	bool 	_sown;
	int	_potential;	
public:
	CultivatedField( GujaratWorld & world, const Engine::Point2D<int> & position );
	virtual ~CultivatedField();
	void sow();
	int harvest();
	bool isSown();
	bool requiresFallow();
	bool isDomesticated();
	int  evaluatePotential() { return _potential; }
	void increasePotential();
	
	const Engine::Point2D<int> getPosition();
};

} // namespace Gujarat

#endif // __CultivatedField_hxx__

