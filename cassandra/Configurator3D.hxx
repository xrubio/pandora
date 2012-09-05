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

#ifndef __Configurator3D_hxx__
#define __Configurator3D_hxx__

#include <QDialog>
#include <ui_Configurator3D.h>

#include <Configuration3D.hxx>

namespace GUI
{
class Configurator3D : public QDialog
{
	Q_OBJECT
	
	Ui::Configurator3D _config3D;
	Configuration3D _configuration;

public:
	Configurator3D(QWidget * parent );
	virtual ~Configurator3D();

private slots:
	void accept();

signals:
	void configured3D( const Configuration3D & );

};

} // namespace GUI

#endif // __Configurator3D_hxx__

