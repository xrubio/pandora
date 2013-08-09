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

#ifndef __RasterConfigurator_hxx__
#define __RasterConfigurator_hxx__

#include <QDialog>

#include <ui_RasterConfigurator.h>

#include <RasterConfiguration.hxx>

namespace GUI
{

class RasterConfigurator : public QDialog
{
	Q_OBJECT

	std::string _type;
	Ui::RasterConfigurator _rasterConfig;

	RasterConfiguration _configuration;

public:
	RasterConfigurator(QWidget * parent, const std::string & type, const std::list<std::string> & orderedRasters );
	virtual ~RasterConfigurator();

private slots:
	void transparentToggled( bool checked );
	void accept();
	void splitColorInterval();
	void removeColorInterval();

signals:
	void rasterConfigured( const std::string &, const RasterConfiguration & );
};

} // namespace GUI

#endif // __RasterConfigurator_hxx__

