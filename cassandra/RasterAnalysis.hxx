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

#ifndef __RasterAnalysis_hxx__
#define __RasterAnalysis_hxx__

#include <QDialog>
#include <ui_RasterAnalysis.h>
#include <tinyxml.h>
#include <GlobalRasterStats.hxx>

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{
class AnalysisControlThread;
class RasterAnalysis : public QDialog
{
	typedef std::map<std::string, int> ValuesMap;
	typedef std::map< QTreeWidgetItem * , ValuesMap > PermutationsMap;

	enum GroupColumn
	{
		eName = 0,
		ePermutations = 1,
		eUse = 2,
		eField = 3
	};

	enum AnalysisType
	{
		eGlobal = 0
		/*,
		eIndividual = 1,
		eHistogram = 2,
		eGeospatial = 3
		*/
	};


	Q_OBJECT

	Ui::RasterAnalysis _analysis;
	PermutationsMap _permutations;

	QPushButton * _runButton;

	std::string _baseDir;
	std::string _outputDir;

	void loadConfigs();
	bool loadConfig( const std::string & configFile );
	void fillParamsTree();

	Engine::SimulationRecord * _sampleRecord;

	void parseLevel( TiXmlNode * parent, QTreeWidgetItem * parentItem);
	void parseAttributes( TiXmlElement * parent, QTreeWidgetItem * parentItem);

	void updateNumberOfPermutations();

	void parseLevelPermutations(TiXmlElement * node, QTreeWidgetItem * item);
	void computePermutations(TiXmlElement * node, QTreeWidgetItem * parentItem );
//	void fillIndividualStats();
//	void fillHistogram();

	void addGlobalAnalysis( AnalysisControlThread* thread );
	/*
	void addIndividualStats( AnalysisControlThread* thread );
	void addHistogram( AnalysisControlThread* thread );
	void addGeoreference( AnalysisControlThread* thread );
	*/

	void groupGlobalStats( PostProcess::GlobalRasterStats * global );

	PostProcess::GlobalRasterStats::Params * _groups;
private slots:
	void selectBaseDir();
	void selectBaseConfig();
	void newAnalysis();
	void removeAnalysis( QWidget * analysis );
	void selectOutput();
	void run();
	void analysisTypeChosen( int index );
	void lastStepChanged( int checked );
public:
	RasterAnalysis( QWidget * parent );
	virtual ~RasterAnalysis();

};

} // namespace GUI

#endif // __RasterAnalysis_hxx__

