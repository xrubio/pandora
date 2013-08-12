/*
 * Copyright (c) 2013
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

#ifndef __Settings_hxx__
#define __Settings_hxx__

#include <ui_Settings.h>

namespace GUI
{

class Settings : public QDialog
{
	Q_OBJECT

	Ui::Settings _settingsUi;

public:
	Settings(QWidget * parent = 0);
	virtual ~Settings();
	void setRadiusSelection( int radiusSelection );

private slots:
	void accept();

signals:
	void radiusSelectionModified(int);

};

} // namespace GUI

#endif // __Settings_hxx__

