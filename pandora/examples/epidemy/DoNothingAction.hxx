
#ifndef __DoNothingAction_hxx__
#define __DoNothingAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine
{
	class Agent;
}

namespace Examples
{

class DoNothingAction : public Engine::Action
{
public:
	DoNothingAction();
	virtual ~DoNothingAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Examples

#endif // __DoNothingAction_hxx__

