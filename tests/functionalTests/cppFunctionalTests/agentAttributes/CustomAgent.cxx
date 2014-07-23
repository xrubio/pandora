
#include <CustomAgent.hxx>

namespace Test
{

CustomAgent::CustomAgent( const std::string & id ) : Agent(id), _strAttribute("not loaded"), _intAttribute(0), _floatAttribute(0.0f)
{
}

CustomAgent::~CustomAgent()
{
}

void CustomAgent::updateState()
{
}

void CustomAgent::registerAttributes()
{
	registerIntAttribute("int attr");
	registerFloatAttribute("float attr");
	registerStringAttribute("str attr");
}
void CustomAgent::serialize()
{
	serializeAttribute("str attr", _strAttribute);
	serializeAttribute("int attr", _intAttribute);
	serializeAttribute("float attr", _floatAttribute);
}

void CustomAgent::setStringAttribute( const std::string & strAttribute)
{
    _strAttribute = strAttribute;
}

void CustomAgent::setIntAttribute( int intAttribute )
{
    _intAttribute = intAttribute;
}

void CustomAgent::setFloatAttribute( float floatAttribute )
{
    _floatAttribute = floatAttribute;
}

} // namespace Test

