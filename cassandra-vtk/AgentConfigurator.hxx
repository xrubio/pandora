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

#ifndef __AgentConfigurator_hxx__
#define __AgentConfigurator_hxx__

#include <QDialog>
#include <QString>
#include <QColor>
#include <QListWidgetItem>
#include <AgentConfiguration.hxx>

#include <ui_AgentConfigurator.h>

class QCloseEvent;
class QIcon;

namespace GUI
{

class AgentConfigurator : public QDialog
{
	Q_OBJECT

//	std::string _selectedShape;
	std::string _type;
	Ui::AgentConfigurator _agentConfig;

	AgentConfiguration _configuration;
public:
	AgentConfigurator(QWidget * parent, const std::string & type );
	virtual ~AgentConfigurator();
	
private slots:
	/*
	void rectangleClicked();
	void circleClicked();
	void squareClicked();
	void triangleClicked();
	void starClicked();
*/
	void selectColor();
	void selectIcon();
	void selectModel3D();

	void colorSelected( const QColor & selectedColor );

	void accept(); 
signals:
	void agentConfigured( const std::string &, const AgentConfiguration & );
};

} // namespace GUI

#endif // __AgentConfigurator_hxx__

