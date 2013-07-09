
#include "BattlefieldConfig.hxx"
#include <tinyxml.h>

namespace ClassicalWarfare
{

BattlefieldConfig::BattlefieldConfig() : _size(0), _initialDistance(9), _redSeparationBetweenLines(0), _blueSeparationBetweenLines(0)
{
}

void BattlefieldConfig::loadArmy(TiXmlElement * element, bool isRed)
{
	if(isRed)
	{
		_redSeparationBetweenLines = atoi(element->Attribute("separationBetweenLines"));
	}
	else
	{
		_blueSeparationBetweenLines = atoi(element->Attribute("separationBetweenLines"));
	}

	TiXmlElement * line = element->FirstChildElement("line");
	while(line)
	{
		Line lineConfig;
		lineConfig._numBlocks = atoi(line->Attribute("numBlocks"));
		lineConfig._ranks = atoi(line->Attribute("ranks"));
		lineConfig._columns = atoi(line->Attribute("columns"));
		lineConfig._separationBetweenBlocks = atoi(line->Attribute("separationBetweenBlocks"));
		lineConfig._offset = atoi(line->Attribute("offset"));

		if(isRed)
		{
			_redLines.push_back(lineConfig);
		}
		else
		{
			_blueLines.push_back(lineConfig);
		}
		line = line->NextSiblingElement("line");
	}
}

void BattlefieldConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("battlefield");
	_size = atoi(element->Attribute("size"));
	_initialDistance = atoi(element->Attribute("initialDistance"));

	loadArmy(root->FirstChildElement("blueArmy"), true);
	loadArmy(root->FirstChildElement("redArmy"), false);
}

BattlefieldConfig::~BattlefieldConfig()
{
}

} // namespace ClassicalWarfare

