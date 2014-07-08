
version = '0.0.1'

import os

installDir = ARGUMENTS.get('installDir', '/usr/local/pandora/')
release = ARGUMENTS.get('release', 1)
extremeDebug = ARGUMENTS.get('edebug', 0)

env = Environment(ENV=os.environ, CXX='mpicxx')
hdf5_path = os.environ.get('HDF5_PATH', '/usr/local/hdf5')

env.VariantDir('build', '.')

libs = Split('pthread gdal hdf5 z tinyxml boost_filesystem boost_system boost_timer boost_chrono gomp mpl dl')

if int(release) == 0:
	env['CCFLAGS'] = Split('-std=c++0x -g -Wall -DTIXML_USE_STL -fopenmp -DPANDORADEBUG')
	if int(extremeDebug)==1:
		env['CCFLAGS'] += ['-DPANDORAEDEBUG']
	libraryName = 'pandorad'
	pythonLibraryName = 'pyPandorad'
else:
	env['CCFLAGS'] = Split('-Ofast -DTIXML_USE_STL -fopenmp -std=c++0x ')
	libraryName = 'pandora'
	pythonLibraryName = 'pyPandora'

coreFiles = [str(f) for f in Glob('src/*.cxx')]
analysisFiles = [str(f) for f in Glob('src/analysis/*.cxx')]
srcFiles = coreFiles + analysisFiles

coreHeaders = [str(f) for f in Glob('include/*.hxx')]
analysisHeaders = [str(f) for f in Glob('include/analysis/*.hxx')]

srcBaseFiles = ['build/' + src for src in srcFiles]
includeDirs = ['include/', 'include/analysis/', '/usr/local/include', hdf5_path + '/include', '/usr/include/gdal/']
libDirs = ['.', '/usr/local/lib', hdf5_path + '/lib']

sharedLib = env.SharedLibrary('lib/'+libraryName, srcBaseFiles, CPPPATH=includeDirs, LIBS=libs, LIBPATH=libDirs, SHLIBVERSION=version)


envPython = Environment(ENV=os.environ, CXX='mpicxx')
envPython['LINKFLAGS'] = Split('-Wl,--export-dynamic,-no-undefined')
envPython['LIBPATH'] = libDirs
envPython['LIBPATH'] += ['/usr/lib/python3.4/config-3.4m-x86_64-linux-gnu/']
envPython.VariantDir('build_py', '.')

if int(release) == 0:
	envPython['CCFLAGS'] = Split('-g -Wall -DTIXML_USE_STL  -std=c++0x -DPANDORADEBUG')	
	if int(extremeDebug)==1:
		envPython['CCFLAGS'] += ['-DPANDORAEDEBUG']
else:
	envPython['CCFLAGS'] = Split('-Ofast -DTIXML_USE_STL -std=c++0x ')

srcPyFiles = ['build_py/' + src for src in srcFiles]
srcPyFiles += [str(f) for f in Glob('src/pyPandora/*.cxx')]

libsPython = libs
includeDirsPython = ['']

conf = Configure(envPython)
if conf.CheckLib('python3.4'):
	libsPython += ['boost_python-py34']
	libsPython += ['python3.4m']
	includeDirsPython = ['/usr/include/python3.4/']
elif conf.CheckLib('python3.2mu'):
	libsPython += ['boost_python-py32']
	libsPython += ['python3.2mu']	
	includeDirsPython = ['/usr/include/python3.2/']
else:
	# default, python 2.7
	libsPython += ['boost_python-py27']
	libsPython += ['python2.7']
	includeDirsPython = ['/usr/include/python2.7/']

includeDirsPython += includeDirs
envPython = conf.Finish()
sharedPyLib = envPython.SharedLibrary('lib/'+pythonLibraryName,  srcPyFiles, CPPPATH=includeDirsPython, LIBS=libsPython, SHLIBVERSION=version)


# installation
installLibDir = installDir + '/lib/'
installHeadersDir = installDir + '/include/'
installAnalysisHeadersDir = installHeadersDir+'analysis'
installedLib = env.InstallVersionedLib(installLibDir, sharedLib, SHLIBVERSION=version)
installedPyLib = env.InstallVersionedLib(installLibDir, sharedPyLib, SHLIBVERSION=version)

installedHeaders = env.Install(installHeadersDir, coreHeaders)
installedAnalysisHeaders = env.Install(installAnalysisHeadersDir, analysisHeaders)

installBin = env.Install(installDir, Glob('./bin'))

env.Alias('install', [installedLib, installedPyLib, installedHeaders, installedAnalysisHeaders, installBin])

# cassandra
cassandraCompilation = env.Command("cassandra/cassandra", "", "cd cassandra && qmake && make")
env.Alias('cassandra', cassandraCompilation)

