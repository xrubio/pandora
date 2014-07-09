
#include <HelloAgent.hxx>
#include <iostream>

HelloAgent::HelloAgent( const std::string & id ) : Agent(id), _value(1)
{
}

HelloAgent::~HelloAgent()
{
}

void HelloAgent::updateState()
{
    std::cout << this << " updating state" << std::endl;
}

void HelloAgent::registerAttributes()
{
    std::cout << this << " registering attributes to be serialized" << std::endl;
	registerIntAttribute("value");
}

void HelloAgent::serialize()
{
    std::cout << this << " serializing" << std::endl;
	serializeAttribute("value", _value);
}

