
This tutorial will explain how to install Pandora framework.

- Requirements
You will need a PC with GNU/Linux installed. The tutorial will explain how to install Pandora on Ubuntu (release 14.04, Trusty Tahr), but any other distribution would work (probably with additional effort related to looking for the correct packages and versions).

- Installing dependencies
First of all, open a console terminal (like Konsole or xterm) and upgrade your system to the last updates:

$ sudo apt-get update
$ sudo apt-get upgrate

Once it is done, this is the full list of external dependencies of Pandora, that need to be installed:

- XML support
libtinyxml-dev

- Cassandra GUI
libdevil-dev
freeglut3-dev
libqwt-dev
libqwt6
libqt4-dev

- GIS support

libgdal1-dev

- MPI support
mpich2

- Compiling System
scons
python

You can install all of them with this command:
$ sudo apt-get install libtinyxml-dev libdevil-dev freeglut3-dev libqwt-dev libqwt6 libqt4-dev libqt4-opengl-dev libgdal1-dev mpich2 scons build-essential libboost-random-dev libboost-python-dev libboost-filesystem-dev libboost-system-dev libmpich2-dev python3 python3-dev libboost-test-dev libboost-timer-dev libboost-chrono-dev

Python 3 is used by default, but if you want to use python 2.7 this is also possible (just change the packages python3 and python3-dev for python2.7 and python2.7-dev).

To make sure that the operating system finds the needed libraries, add library directories that are not standard to LD_LIBRARY_PATH external variable. It can be done adding this line to the end of the file .bashrc inside your $HOME directory.

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/hdf5/lib/

- Compiling HDF5
Ubuntu dependencies forces you to install the serial version of HDF5 as gdaldependency. For this reason, we will need to download, compile and install hdf5.
	* Go to HDF5 webpage and download the sources:
		$ cd ~/
		$ mkdir hdf5
		$ cd hdf5
		$ wget "http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-1.8.17.tar.bz2"

	* Unzip the file:
		$ tar jxvf hdf5-1.8.17.tar.bz2

	* configure the library:
		$ cd hdf5-1.8.17
		$ ./configure --enable-parallel --prefix=/usr/local/hdf5 --disable-shared --with-pic
		(if you want more information type ./configure --help)

	* compile and install:
		$ make
		$ sudo make install

- Compiling Pandora
	* Go to the folder where Pandora is deployed:
		$ cd PATHTOPANDORA/pandora/
	* Compile:
		$ scons
		- if you want to compile in debug mode type: $ scons debug=1 
        - if you want to use python2.7 type: $scons python2
        - you can check the full list of options with: $ scons -h

- Installing Pandora
	* Go to the folder where Pandora is deployed:
		$ cd PATHTOPANDORA/pandora/
	* execute:
        $ sudo scons install (providing root password)
        By default Pandora will be installed in /usr/local/pandora. If you want to use a different directory use the option installDir=$PATH, such ash:
        $ sudo scons install installDir=/opt/

    * You will need to add Pandora directory to your .bashrc, using these lines:
        export PANDORAPATH=/usr/local/pandora
        export PATH=$PATH:$PANDORAPATH/bin/
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PANDORAPATH/lib/
        
        Please change PANDORAPATH if you used a different installation directory.

- Test with an example
	* Go to any example or app and compile it:
		$ cd examples/randomWalkers/
		$ scons
	
	* Execute it:
		$ ./randomWalkers
	
- Compiling cassandra
    * Go to the folder where Pandora is deployed:
		$ cd PATHTOPANDORA/pandora/
	* execute:
        $ scons cassandra
	* Execute it:
		$ ./bin/cassandra
	* Finally, check whether the results of randomWalkers (they should be inside a 'data' directory) are loaded correctly in cassandra.

        
