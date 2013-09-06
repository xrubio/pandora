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

#include <TraitAnalysis.hxx>
#include <SimulationRecord.hxx>

namespace GUI
{

TraitAnalysis::TraitAnalysis( QWidget * parent, Engine::SimulationRecord * record, const std::string & type ) : QWidget(parent), _record(record)
{
	_traits.setupUi(this);

	// sample agent
	Engine::SimulationRecord::AgentRecordsMap::const_iterator it=_record->beginAgents(type);

	QStringList traits;

	Engine::AgentRecord * agentRecord = it->second;
	for(Engine::AgentRecord::StatesMap::const_iterator itS=agentRecord->beginStates(); itS!=agentRecord->endStates(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
	_traits.traits->addItems(traits);

	/*
	QStringList analysis;
	analysis << "Mean";
	analysis << "Sum";
	analysis << "Standard Deviation";
	*/

	_traits.analysis->addItem("Mean", TraitAnalysis::eMean);
	_traits.analysis->addItem("Sum", TraitAnalysis::eSum);
	_traits.analysis->addItem("Standard Deviation", TraitAnalysis::eStandardDeviation);

	connect(_traits.analysis, SIGNAL(activated(const QString &)), this, SLOT(analysisSelected(const QString &)));
	connect(_traits.removeButton, SIGNAL(clicked()), this, SLOT(removeAnalysis()));
}
	
TraitAnalysis::~TraitAnalysis()
{
}

void TraitAnalysis::removeAnalysis()
{
	emit removeAnalysis(this);
}

void TraitAnalysis::analysisSelected( const QString & option)
{
	// TODO change widget depending on option
	/*
	if(option.isEmpty())
	{
		return;
	}
	_traits.traits->setEnabled(true);
	*/
}

TraitAnalysis::GlobalAnalysis TraitAnalysis::getAnalysis() const
{
	return (GlobalAnalysis)_traits.analysis->findData(_traits.analysis->currentIndex()); //currentText().toStdString();
}

std::string TraitAnalysis::getTrait() const
{
	return _traits.traits->currentText().toStdString();
}

} // namespace GUI

