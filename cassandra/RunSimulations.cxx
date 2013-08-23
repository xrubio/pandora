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

#include <RunSimulations.hxx>
#include <QPushButton>

namespace GUI
{

RunSimulations::RunSimulations( QWidget * parent ) : QDialog(parent)
{
	setModal(true);
	_run.setupUi(this);
	_run.status->setText("running!");
	_doneButton = _run.buttonBox->addButton("Done", QDialogButtonBox::AcceptRole);
}

RunSimulations::~RunSimulations()
{
}

void RunSimulations::init( int numberOfExperiments )
{
	_run.progressBar->setRange(0, numberOfExperiments);
	_run.progressBar->setValue(0);
	QString message("executing run: ");
	message.append(QString::number(1+_run.progressBar->value())+"/"+QString::number(_run.progressBar->maximum()));
	_run.status->setText(message);
	_doneButton->setEnabled(false);
}

void RunSimulations::updateSimulationRun()
{
	int nextValue = 1+_run.progressBar->value();
	_run.progressBar->setValue(nextValue);
	
	if(nextValue==_run.progressBar->maximum())
	{
		_run.status->setText("Finished!");
		_doneButton->setEnabled(true);
	}
	else
	{
		QString message("executing run: ");
		message.append(QString::number(1+_run.progressBar->value())+"/"+QString::number(_run.progressBar->maximum()));
		_run.status->setText(message);
	}
	update();
}

} // namespace GUI

