
version = '0.0.1'

import os

installDir = ARGUMENTS.get('installDir', '/usr/local/pandora/')
release = ARGUMENTS.get('release', 1)
extremeDebug = ARGUMENTS.get('edebug', 0)

AddOption('--with-python2', dest='usePython2', default=False, action='store_true', help='use Python2.7 instead of Python3')

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
envPython.Append(LINKFLAGS = '''-Wl,--export-dynamic,-no-undefined'''.split())
envPython.Append(LIBPATH = libDirs)
envPython.Append(LIBS = libs)
envPython.Append(CPPPATH= includeDirs)

envPython.VariantDir('build_py', '.')

if int(release) == 0:
    envPython['CCFLAGS'] = Split('-g -Wall -DTIXML_USE_STL  -std=c++0x -DPANDORADEBUG') 
    if int(extremeDebug)==1:
        envPython['CCFLAGS'] += ['-DPANDORAEDEBUG']
else:
    envPython['CCFLAGS'] = Split('-Ofast -DTIXML_USE_STL -std=c++0x ')

srcPyFiles = ['build_py/' + src for src in srcFiles]
srcPyFiles += [str(f) for f in Glob('src/pyPandora/*.cxx')]
srcPyFiles += [str(f) for f in Glob('utils/*.cxx')]

conf = Configure(envPython)

if(GetOption('usePython2')==False):
    envPython.ParseConfig("pkg-config python3 --cflags --libs")
    if conf.CheckLib('boost_python-py33'):
        envPython.Append(LIBS = 'boost_python-py33')
    elif conf.ChecLib('boost_python-py34'):
        envPython.Append(LIBS = 'boost_python-py34')
else:
    envPython.ParseConfig("pkg-config python2 --cflags --libs")
    envPython.Append(LIBS = 'boost_python-py27')

envPython = conf.Finish()
sharedPyLib = envPython.SharedLibrary('lib/'+pythonLibraryName,  srcPyFiles, SHLIBVERSION=version)


# installation
installLibDir = installDir + '/lib/'
installHeadersDir = installDir + '/include/'
installAnalysisHeadersDir = installHeadersDir+'analysis'
installedLib = env.InstallVersionedLib(installLibDir, sharedLib, SHLIBVERSION=version)
installedPyLib = env.InstallVersionedLib(installLibDir, sharedPyLib, SHLIBVERSION=version)

installedHeaders = env.Install(installHeadersDir, coreHeaders)
installedAnalysisHeaders = env.Install(installAnalysisHeadersDir, analysisHeaders)

installBin = env.Install(installDir, Glob('./bin'))
installMpiStub = env.Install(installDir+'/utils', Glob('./utils/*.cxx'))

# cassandra
cassandraCompilation = env.Command("bin/cassandra", "", "cd cassandra && qmake && make")

# final targets
Default(sharedLib)
Default(sharedPyLib)
env.Alias('cassandra', cassandraCompilation)
env.Alias('install', [cassandraCompilation, sharedLib, sharedPyLib, installedLib, installedPyLib, installedHeaders, installedAnalysisHeaders, installBin, installMpiStub])

