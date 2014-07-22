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

#include <QApplication>

#include <MainWindow.hxx>
#include <Exception.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	srand(time(0));
	try
	{
        if(argc>2)
		{
			throw Engine::Exception("USAGE: cassandra [simulation file.h5]");
		}		

		QApplication::setGraphicsSystem("raster");
		QApplication app(argc, argv);
		GUI::MainWindow mainWindow;
		if(argc!=1)
        {
            std::string simulation = argv[1];
            mainWindow.selectSimulation(simulation,1);
        }
		mainWindow.show();
		return app.exec();
	}
	catch( Engine::Exception & exceptionThrown )
	{
		std::cout << "app exception thrown: " << exceptionThrown.what() << std::endl;
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "std exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

