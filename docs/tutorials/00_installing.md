
This tutorial will explain how to install Pandora framework.

- Requirements
You will need a PC with GNU/Linux installed. The tutorial will explain how to install Pandora on Ubuntu (release 14.04, Trusty Tahr), but any other distribution would work (probably with additional effort related to looking for the correct packages and versions).

- Installing dependencies
First of all, open a console terminal (like Konsole or xterm) and upgrade your system to the last updates:
```bash
 $ sudo apt-get update
 $ sudo apt-get upgrate
```

Once it is done, this is the full list of external dependencies of Pandora, that need to be installed:

- XML support
```bash
 libtinyxml-dev
```

- Cassandra GUI
```bash
 libdevil-dev
 freeglut3-dev
 libqwt-dev
 libqwt6
 libqt4-dev
```

- GIS support

```bash
libgdal1-dev
```

- MPI support
```bash
mpich2
```

- Compiling System
```bash
scons
python
```

You can install all of them with this command:
```bash
$ sudo apt-get install libtinyxml-dev libdevil-dev freeglut3-dev libqwt-dev libqwt6 libqt4-dev libqt4-opengl-dev libgdal1-dev mpich2 scons build-essential libboost-random-dev libboost-python-dev libboost-filesystem-dev libboost-system-dev libmpich2-dev python3 python3-dev libboost-test-dev libboost-timer-dev libboost-chrono-dev
```

Python 3 is used by default, but if you want to use python 2.7 this is also possible (just change the packages python3 and python3-dev for python2.7 and python2.7-dev).

To make sure that the operating system finds the needed libraries, add library directories that are not standard to `LD_LIBRARY_PATH` external variable. It can be done adding this line to the end of the file .bashrc inside your `$HOME` directory.

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/hdf5/lib/
```

- Compiling HDF5
Ubuntu dependencies forces you to install the serial version of HDF5 as gdaldependency. For this reason, we will need to download, compile and install hdf5.
	* Go to HDF5 webpage and download the sources:
	```bash
	$ cd ~/
	$ mkdir hdf5
	$ cd hdf5
	$ wget "http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-1.8.17.tar.bz2"
	```
	* Unzip the file:
	```bash
	$ tar jxvf hdf5-1.8.17.tar.bz2
	```
	* configure the library:
	```bash
	$ cd hdf5-1.8.17
	$ ./configure --enable-parallel --prefix=/usr/local/hdf5 --disable-shared --with-pic
	#(if you want more information type ./configure --help11)
 	```
	* compile and install:
	```bash
	$ make
	$ sudo make install
	```

- Compiling Pandora
  1. Go to the folder where Pandora is deployed:
  ```bash
  $ cd PATHTOPANDORA/pandora/
  ```
  2. Compile:
  ```bash
  $ scons
  ```
	- if you want to compile in debug mode type: 
	```bash 
	$ scons debug=1
	``` 
	- if you want to use python2.7 type: 
	```bash 
	$scons python2 
	```
	- you can check the full list of options with: 
	```bash
	$ scons -h
	```

- Installing Pandora
	* Go to the folder where Pandora is deployed:
	```bash
	$ cd PATHTOPANDORA/pandora/
	```
	* execute:
	```bash
        $ sudo scons install (providing root password)
	```
	By default Pandora will be installed in `/usr/local/pandora`. If you want to use a different directory use the option `installDir=$PATH`, such ash:
	```bash
        $ sudo scons install installDir=/opt/
	```

	* You will need to add Pandora directory to your .bashrc, using these lines:
	```bash
        export PANDORAPATH=/usr/local/pandora
        export PATH=$PATH:$PANDORAPATH/bin/
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PANDORAPATH/lib/
	```
	Please change `PANDORAPATH` if you used a different installation directory.

- Test with an example
	* Go to any example or app and compile it:
	```bash
	$ cd examples/randomWalkers/
	$ scons
	```
	
	* Execute it:
	```bash
	$ ./randomWalkers
	```
	
- Compiling cassandra
	* Go to the folder where Pandora is deployed:
	```bash
	$ cd PATHTOPANDORA/pandora/
	```
	* execute:
	```bash
        $ scons cassandra
	```
	* Execute it:
	```bash
	$ ./bin/cassandra
	```
	* Finally, check whether the results of `randomWalkers` (they should be inside a 'data' directory) are loaded correctly in cassandra.

        
[Next - Get Started with pyPandora](01_getting_started_pyPandora.md)
Or [Next - Get Started with C++](02_getting_started_pandora.md)
