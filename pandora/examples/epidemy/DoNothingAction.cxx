
#include <DoNothingAction.hxx>

#include <Agent.hxx>

namespace Examples
{

DoNothingAction::DoNothingAction()
{
}

DoNothingAction::~DoNothingAction()
{
}

void DoNothingAction::execute( Engine::Agent & agent )
{	
}

std::string DoNothingAction::describe() const
{
	return "DoNothingAction";
}

} // namespace Examples

