/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef __AgentConfiguration_hxx__
#define __AgentConfiguration_hxx__

#include <QIcon>
#include <QColor>
#include <Point3D.hxx>

namespace GUI
{
class Model3D;

class AgentConfiguration
{
	QColor _color;
	QIcon * _icon;
	bool _useIcon;
	std::string _fileName2D;
	float _size;
    bool _showValue;

	Engine::Point3D<float> _size3D;
	std::string _fileName3D;
	Model3D * _model;
		
public:
	AgentConfiguration();
	AgentConfiguration( const AgentConfiguration & prototype );
	virtual ~AgentConfiguration();

	void setColor( const QColor & color );
	//void setIcon( const std::string & fileName );
	void setUseIcon( const bool & useIcon );
	void setSize( const float & size );
	void setSize3D( const Engine::Point3D<float> & size3D );
	void setFileName2D( const std::string & fileName2D);
	void setFileName3D( const std::string & fileName3D);
    void showValue( const bool & showValue ) { _showValue = showValue; }

	const bool & useIcon() const;
	const QColor & getColor() const;
	const QIcon & getIcon() const;
	const float & getSize() const;
	const Engine::Point3D<float> & getSize3D() const;
	const std::string & getFileName2D() const;
	const std::string & getFileName3D() const;
	const Model3D & getModel() const;
    const bool & showValue() const { return _showValue; }
};

} // namespace GUI

#endif // __AgentConfiguration_hxx__

