
#include <AgroPastoralist.hxx>
#include <GujaratWorld.hxx>
#include <CultivatedField.hxx>
#include <Exceptions.hxx>

#include <APMoveHomeAction.hxx>
#include <AbandonPlotAction.hxx>
#include <EstablishPlotAction.hxx>
#include <SowAction.hxx>
#include <MaintainPlotAction.hxx>
#include <HarvestAction.hxx>
#include <Agent.hxx>
#include <Action.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace Gujarat
{

AgroPastoralist::AgroPastoralist( const std::string & id ) : GujaratAgent(id), _maxCropHomeDistance(10), _cultivatedField(0)
{
}

AgroPastoralist::~AgroPastoralist()
{
}

void AgroPastoralist::updateKnowledge()
{
}

bool AgroPastoralist::cultivatedFieldOutOfReach()
{
	return _position.distance(_cultivatedField->getPosition()) > getMaxCropHomeDistance();
}

void AgroPastoralist::selectActions()
{
	if(_cultivatedField == NULL || _cultivatedField->requiresFallow() )
	{
		std::vector< EstablishPlotAction* > possibleEPActions;
		EstablishPlotAction::generatePossibleActions( *this, possibleEPActions );

		if ( possibleEPActions.empty() )
		{
			std::vector< APMoveHomeAction* > possibleMHActions;
			APMoveHomeAction::generatePossibleActions( *this, possibleMHActions );
			
			int dice = Engine::GeneralState::statistics().getUniformDistValue( 0, possibleMHActions.size() - 1 );
	
			APMoveHomeAction* selectedAction = possibleMHActions[dice];
			possibleMHActions[dice] = NULL;
			_actions.push_back( selectedAction );
			for ( unsigned i = 0; i < possibleMHActions.size(); i++ )
				if ( possibleMHActions[i] != NULL )
					delete possibleMHActions[i];
			return;
		}


		int dice = Engine::GeneralState::statistics().getUniformDistValue( 0, possibleEPActions.size() - 1 );
		
		EstablishPlotAction* selectedAction = possibleEPActions[dice];
		possibleEPActions[dice] = NULL;
		_actions.push_back( selectedAction );
		for ( unsigned i = 0; i < possibleEPActions.size(); i++ )
			if ( possibleEPActions[i] != NULL )
				delete possibleEPActions[i];
		return;
	}

	// MRJ: This is a HORRIBLE thing to do!!!
	GujaratWorld* world = dynamic_cast<GujaratWorld*>( _world );

	if( world->isColdDrySeason() && _cultivatedField->isDomesticated() && !_cultivatedField->isSown() )
	{
		_actions.push_back(new SowAction());
		return;
	}

	if ( _cultivatedField->isSown() && _cultivatedField->evaluatePotential() < 80 )
	{
		int dice = Engine::GeneralState::statistics().getUniformDistValue( 0, 10 );
		_actions.push_back(new MaintainPlotAction());
		return;
	}

	if ( _cultivatedField->isSown() && _cultivatedField->evaluatePotential() >= 80 )
		_actions.push_back( new HarvestAction() );
}

void * AgroPastoralist::createPackage()
{
	return 0;
}

GujaratAgent * AgroPastoralist::createNewAgent()
{	
	GujaratWorld * world = (GujaratWorld*)_world;
	std::ostringstream oss;
	oss << "AgroPastoralist_" << world->getId() << "-" << world->getNewKey();
	return new AgroPastoralist(oss.str());
}

void AgroPastoralist::abandonCultivatedField()
{
	_world->setValue("resourceType", _cultivatedField->getPosition(), FALLOW);
	delete _cultivatedField;
}

void AgroPastoralist::acquireCultivatedField( Engine::Point2D<int> p )
{
	if ( _cultivatedField != NULL )
		abandonCultivatedField();
	_cultivatedField = new CultivatedField( (GujaratWorld&)(*_world), p );
}

} // namespace Gujarat

