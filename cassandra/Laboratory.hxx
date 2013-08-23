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

#ifndef __Laboratory_hxx__
#define __Laboratory_hxx__

#include <QDialog>
#include <ui_Laboratory.h>
#include <tinyxml.h>
#include <map>
#include <vector>


namespace GUI
{

enum ParamColumn
{
	eName = 0,
	eText = 1,
	eMin = 2,
	eMax = 3,
	eStep = 4
};

class RunSimulations;

class Laboratory : public QDialog
{

	typedef std::map< QTreeWidgetItem *, std::string> ParamsMap;
	typedef std::vector<ParamsMap> RunsVector;

private:
	Q_OBJECT
	Ui::Laboratory _lab;

	std::string _simulationBinary;
	std::string _simulationConfig;
	std::string _outputDir;

	RunsVector _runs;

	void parseLevel( TiXmlNode * parent, QTreeWidgetItem * parentItem);
	void parseAttributes( TiXmlElement * parent, QTreeWidgetItem * parentItem);

	void generateConfigs();
	// fill _runs with possible experiments
	void computeExperiments();
	// creates a config file at dir filling it with parameters stored at index of _runs
	void createConfigFile( const std::string & dir, int index );
	void runSimulations();

	// returns true if xml was correctly loaded
	bool parseConfig();
	// returns the number of executions if config is correct, or -1 in other case
	void updateNumberOfExecutions();

	// recursive method that stores the children of an element for index run
	void storeChildren(TiXmlElement * parentElement, QTreeWidgetItem * parentItem, int index);

	QPushButton * _generateButton;
	QPushButton * _runButton;
	// number of experiments to be performed (without repeats)
	int _numExperiments;
	RunSimulations * _runSimulations;
public:
	Laboratory(QWidget * parent);
	virtual ~Laboratory();

private slots:
	void selectSimulation();
	void selectConfig();
	void selectOutput();
	void showConfig();
	void paramChanged( QTreeWidgetItem * item, int column );
	void doubleClick(QTreeWidgetItem * item, int column);
	void buttonClicked( QAbstractButton * button );
	void numRepeatsChanged(int numRepeats);

signals:
	void nextSimulation();
};

}

#endif // __Laboratory_hxx__

