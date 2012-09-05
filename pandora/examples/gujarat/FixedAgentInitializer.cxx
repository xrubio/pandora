#include <FixedAgentInitializer.hxx>
#include <tinyxml.h>
#include <cstdlib>
#include <cassert>
#include <GujaratAgent.hxx>

namespace Gujarat
{

FixedAgentInitializer::FixedAgentInitializer( TiXmlElement* root )
	: AgentInitializer( root )
{
	TiXmlNode* n = NULL;
	while ( ( n = root->IterateChildren(n) ) )
	{
		if ( n->Type() != TiXmlNode::TINYXML_ELEMENT ) continue;
		TiXmlElement* elem = n->ToElement();
		if (!elem->ValueStr().compare( "individual" ))
		{
			int value = atoi( elem->Attribute("age") );
			_individualsAges.push_back( value );
		}
	}

}

FixedAgentInitializer::~FixedAgentInitializer()
{
}

void	FixedAgentInitializer::initialize( GujaratAgent* agent )
{
	for ( unsigned k = 0; k < _individualsAges.size(); k++ )
		agent->addNewIndividual( _individualsAges[k] );
}

}
