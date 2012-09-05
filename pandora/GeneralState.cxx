
#include <GeneralState.hxx>

namespace Engine
{

GeneralState * GeneralState::_instance = 0;

GeneralState & GeneralState::instance()
{
	if(!_instance)
	{
		_instance = new GeneralState;		
	}
	return *_instance;
}

GeneralState::GeneralState()
{
}

GeneralState::~GeneralState()
{

}

} // namespace Engine

