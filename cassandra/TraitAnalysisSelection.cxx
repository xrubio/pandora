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

#include <TraitAnalysisSelection.hxx>
#include <SimulationRecord.hxx>

namespace GUI
{

TraitAnalysisSelection::TraitAnalysisSelection( QWidget * parent, Engine::SimulationRecord * record, const std::string & type ) : QWidget(parent)
{
	_selection.setupUi(this);

	// sample agent
	Engine::SimulationRecord::AgentRecordsMap::const_iterator it=record->beginAgents(type);

	QStringList traits;

	Engine::AgentRecord * agentRecord = it->second;
	for(Engine::AgentRecord::IntAttributesMap::const_iterator itS=agentRecord->beginInt(); itS!=agentRecord->endInt(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
    for(Engine::AgentRecord::FloatAttributesMap::const_iterator itS=agentRecord->beginFloat(); itS!=agentRecord->endFloat(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
	for(Engine::AgentRecord::StrAttributesMap::const_iterator itS=agentRecord->beginStr(); itS!=agentRecord->endStr(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
	_selection.traits->addItems(traits);

	/*
	QStringList analysis;
	analysis << "Mean";
	analysis << "Sum";
	analysis << "Standard Deviation";
	*/

	_selection.analysis->addItem("Mean", TraitAnalysisSelection::eMean);
	_selection.analysis->addItem("Sum", TraitAnalysisSelection::eSum);
	_selection.analysis->addItem("Standard Deviation", TraitAnalysisSelection::eStandardDeviation);

	connect(_selection.analysis, SIGNAL(activated(const QString &)), this, SLOT(analysisSelected(const QString &)));
	connect(_selection.removeButton, SIGNAL(clicked()), this, SLOT(removeAnalysis()));
}
	
TraitAnalysisSelection::~TraitAnalysisSelection()
{
}

void TraitAnalysisSelection::removeAnalysis()
{
	emit removeAnalysis(this);
}

void TraitAnalysisSelection::analysisSelected( const QString & )
{
	// TODO change widget depending on option
	/*
	if(option.isEmpty())
	{
		return;
	}
	_selection.traits->setEnabled(true);
	*/
}

TraitAnalysisSelection::GlobalAnalysis TraitAnalysisSelection::getAnalysis() const
{
	return (GlobalAnalysis)_selection.analysis->findData(_selection.analysis->currentIndex());
}

std::string TraitAnalysisSelection::getTrait() const
{
	return _selection.traits->currentText().toStdString();
}

} // namespace GUI

