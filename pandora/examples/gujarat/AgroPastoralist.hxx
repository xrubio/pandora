
#ifndef __AgroPastoralist_hxx__
#define __AgroPastoralist_hxx__

#include <GujaratAgent.hxx>

namespace Gujarat
{

class CultivatedField;

class AgroPastoralist : public GujaratAgent
{
	int _maxCropHomeDistance;
	
	void updateKnowledge();

	void selectActions();
	
	GujaratAgent * createNewAgent();

	CultivatedField * _cultivatedField;
public:
	AgroPastoralist( const std::string & id );
	virtual ~AgroPastoralist();

	void * createPackage();

	void moveHome();
	void establishPlot();
	void sow();
	void maintainPlot();
	void harvest();
	void abandonPlot();
	void acquireCultivatedField( Engine::Point2D<int> p );
	void abandonCultivatedField();
	bool hasCultivatedField() { return _cultivatedField != NULL; }
	bool cultivatedFieldOutOfReach();
	void acquireResources( int amount ) { _collectedResources += amount; }

	void serialize(){}

	void	setMaxCropHomeDistance( int v ) { _maxCropHomeDistance = v; }
	int	getMaxCropHomeDistance() const { return _maxCropHomeDistance; }
	bool	isColdDrySeason();
	CultivatedField& getCultivatedField() { return *_cultivatedField; }

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	AgroPastoralist( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Gujarat

#endif // __AgroPastoralist_hxx__

