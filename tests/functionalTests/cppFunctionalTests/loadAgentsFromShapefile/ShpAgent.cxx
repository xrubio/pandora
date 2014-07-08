
#include <ShpAgent.hxx>

namespace Test
{

ShpAgent::ShpAgent( const std::string & id ) : Agent(id), _label("label not loaded"), _intValue(0), _floatValue(0.0f)
{
}

ShpAgent::~ShpAgent()
{
}

void ShpAgent::updateState()
{
}

void ShpAgent::registerAttributes()
{
	registerIntAttribute("intValue");
	registerIntAttribute("floatValue");
	registerStringAttribute("label");
}
void ShpAgent::serialize()
{
	serializeAttribute("label", _label);
	serializeAttribute("intValue", _intValue);
	serializeAttribute("floatValue", _floatValue);
}

void ShpAgent::setLabel( const std::string & label )
{
	_label = label;
}

void ShpAgent::setIntValue( int intValue )
{
	_intValue = intValue;
}

void ShpAgent::setFloatValue( float floatValue )
{
	_floatValue = floatValue;
}

const std::string ShpAgent::getLabel()
{
	return _label;
}

int ShpAgent::getIntValue()
{
	return _intValue;
}

float ShpAgent::getFloatValue()
{
	return _floatValue;
}

} // namespace Test

