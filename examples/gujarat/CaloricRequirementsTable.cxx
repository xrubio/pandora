#include <CaloricRequirementsTable.hxx>
#include <tinyxml.h>
#include <cstdlib>
#include <cassert>

namespace Gujarat
{

CaloricRequirementsTable::CaloricRequirementsTable( TiXmlElement* root )
{
	TiXmlNode* n = NULL;
	while ( ( n = root->IterateChildren(n) ) )
	{
		if ( n->Type() != TiXmlNode::TINYXML_ELEMENT ) continue;
		TiXmlElement* elem = n->ToElement();
		std::cout << "elem->ValueStr() = " << elem->ValueStr() << std::endl;	
		if ( !elem->ValueStr().compare( "entry" ) )
		{
			int ageMin = atoi( elem->Attribute("ageMin") );
			int ageMax = atoi( elem->Attribute("ageMax") );
			int value = atoi( elem->Attribute("value") );	
			_ageBrackets.push_back( std::make_pair(ageMin, ageMax) );
			_values.push_back( value );	
		}
	}
}

CaloricRequirementsTable::~CaloricRequirementsTable()
{
}

int CaloricRequirementsTable::getCaloriesFor( int age )
{
	for ( unsigned k = 0; k < _ageBrackets.size(); k++ )
	{
		if ( age < _ageBrackets[k].first ||
			age >= _ageBrackets[k].second )
			continue;
		return _values[k]; 
	}
	return 0;	
}

}
