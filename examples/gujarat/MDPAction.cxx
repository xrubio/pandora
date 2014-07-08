#include <MDPAction.hxx>
#include <Exception.hxx>
#include <iostream>
#include <HunterGathererMDPState.hxx>

namespace Gujarat
{

MDPAction::MDPAction()
{
}

MDPAction::~MDPAction()
{
}

void MDPAction::executeMDP( const GujaratAgent& agent, const HunterGathererMDPState& s, 
			HunterGathererMDPState& sp ) const
{
	throw Engine::Exception( "Not Implemented for AgroPastoralist MDP states!" );
}

} // namespace Gujarat

