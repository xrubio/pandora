
1. Install homebrew
ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"

2. Open a terminal and Install git
brew install git

3. Download pandora source code
git clone https://github.com/xrubio/pandora

4. download and install xquartz from: http://xquartz.macosforge.org

5.Install pandora dependencies
  brew install gcc
  HOMEBREW_CC=gcc-4.8 HOMEBREW_CXX=g++-4.8 brew install tinyxml devil qt gdal scons python3
  brew install qwt
  HOMEBREW_CC=gcc-4.8 HOMEBREW_CXX=g++-4.8 brew install mpich2
  HOMEBREW_CC=gcc-4.8 HOMEBREW_CXX=g++-4.8 brew install boost --c++11 --with-python3
  HOMEBREW_CC=gcc-4.8 HOMEBREW_CXX=g++-4.8 brew install hdf5 --enable-parallel

Python 3 is used by default, but if you want to use python 2.7 this is also possible (just don't use the 'with-python3' flags for boost).

6. To make sure that the operating system finds the needed libraries, add library directories that are not standard to `DYLD_LIBRARY_PATH` external variable. It can be done adding this line to the end of the file .bashrc inside your $HOME directory. You should also add Pandora directory

 export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/xrubio/workspace/pandora/pandora/


7. Compiling pandora:
    * Go to the folder where Pandora is deployed:
		`$ cd PATHTOPANDORA/pandora/`
	* Compile:
		$ scons
		- if you want to compile in debug mode type: $ scons debug=1 
        - if you want to use python2.7 type: $scons python2
        - you can check the full list of options with: $ scons -h

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

- Installing Pandora
     * Go to the folder where Pandora is deployed:
		$ cd PATHTOPANDORA/pandora/
     * execute:
        $ sudo scons install (providing root password)
        By default Pandora will be installed in /usr/local/pandora. If you want to use a different directory use the option installDir=$PATH, such ash:
        $ sudo scons install installDir=/opt/

    * You will need to add Pandora directory to your .bash_profile, using these lines:
        export PANDORAPATH=/usr/local/pandora/
        export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PANDORAPATH/lib
        export PATH=$PATH:$PANDORAPATH/bin
        export PATH=$PATH:$PANDORAPATH/bin/cassandra.app/Contents/MacOS
        
        Please change PANDORAPATH if you used a different installation directory.

