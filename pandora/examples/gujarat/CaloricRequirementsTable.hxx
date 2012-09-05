#ifndef __CALORIC_REQUIREMENTS_TABLE__
#define __CALORIC_REQUIREMENTS_TABLE__

#include <vector>
#include <utility>

class TiXmlElement;

namespace Gujarat
{

class CaloricRequirementsTable
{
public:
	CaloricRequirementsTable( TiXmlElement* elem );
	~CaloricRequirementsTable();

	int getCaloriesFor( int age );

protected:

	std::vector<std::pair<int,int> > 	_ageBrackets;
	std::vector<int>			_values;

};

}
#endif // CaloricRequirementsTable.hxx
