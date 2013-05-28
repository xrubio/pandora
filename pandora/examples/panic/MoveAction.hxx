
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <Point2D.hxx>
#include <string>

namespace Engine
{
	class Agent;
}

namespace Panic 
{

class MoveAction : public Engine::Action
{
	Engine::Point2D<int> _position;
	float _agentCompressionWeight;
	float _wallCompressionWeight;
	int _panicRadius;
public:
	MoveAction( const Engine::Point2D<int> & position, float agentCompressiontWeight, float wallCompressionWeight, int panicRadius );
	virtual ~MoveAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Panic

#endif // __MoveHomeAction_hxx__

