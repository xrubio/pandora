
version = '0.0.1'

import os, platform, SCons
from distutils.version import LooseVersion

vars = Variables('custom.py')
vars.Add(BoolVariable('debug', 'compile with debug flags', 'no'))
vars.Add(BoolVariable('edebug', 'compile with extreme debug logs', 'no'))
vars.Add(BoolVariable('python2', 'use Python2.7 instead of Python3', 'no'))
vars.Add('installDir', 'directory where to install Pandora', '/usr/local/pandora')

if platform.system()=='Linux':
    vars.Add(PathVariable('hdf5', 'Path where HDF5 library was installed', '/usr/local/hdf5', PathVariable.PathIsDir))

env = Environment(variables=vars, ENV=os.environ, CXX='mpicxx')
if env['debug'] == False:
    env.VariantDir('build', '.')
else:
    env.VariantDir('buildDebug', '.')

Help(vars.GenerateHelpText(env))

env.Append(LIBS = 'pthread gdal hdf5 z tinyxml boost_filesystem boost_system boost_timer boost_chrono gomp mpl dl'.split())

env.Append(CCFLAGS = '-DTIXML_USE_STL -fopenmp -std=c++0x')

if platform.system()=='Darwin':
    env.Append(CCFLAGS = '-cxx=g++-4.8')
    env.Append(LINKFLAGS = '-undefined warning')

if env['debug'] == True:
    env.Append(CCFLAGS = '-g -Wall -DPANDORADEBUG')
    if env['edebug']==True:
        env.Append(CCFLAGS = '-DPANDORAEDEBUG')
    libraryName = 'pandorad.so'
    pythonLibraryName = 'pyPandorad.so'
else:
    env.Append(CCFLAGS = '-Ofast')
    libraryName = 'pandora.so'
    pythonLibraryName = 'pyPandora.so'

coreFiles = [str(f) for f in Glob('src/*.cxx')]
analysisFiles = [str(f) for f in Glob('src/analysis/*.cxx')]
srcFiles = coreFiles + analysisFiles

coreHeaders = [str(f) for f in Glob('include/*.hxx')]
analysisHeaders = [str(f) for f in Glob('include/analysis/*.hxx')]

if env['debug'] == False:
    srcBaseFiles = ['build/' + src for src in srcFiles]
else:
    srcBaseFiles = ['buildDebug/' + src for src in srcFiles]

env.Append(CPPPATH = 'include include/analysis'.split())

if platform.system()=='Linux':
    env.Append(CPPPATH = [env['hdf5']+'/include', '/usr/include/gdal/'])
    env.Append(CPPPATH = [env['hdf5']+'/include'])
    env.Append(LIBPATH = [env['hdf5']+'/lib'])
elif platform.system()=='Darwin':
    env.Append(LIBPATH = '/usr/local/lib')


envPython = env.Clone()

if platform.system()=='Linux':
    envPython.Append(LINKFLAGS = '-Wl,--export-dynamic,-no-undefined'.split())

if env['debug'] == False:
    envPython.VariantDir('build_py', '.')
    srcPyFiles = ['build_py/' + src for src in srcFiles]
else:    
    envPython.VariantDir('buildDebug_py', '.')
    srcPyFiles = ['buildDebug_py/' + src for src in srcFiles]

srcPyFiles += [str(f) for f in Glob('src/pyPandora/*.cxx')]
srcPyFiles += [str(f) for f in Glob('utils/*.cxx')]

conf = Configure(envPython)

if(env['python2']==False):
    envPython.ParseConfig("python3-config --includes --ldflags")
    if conf.CheckLib('boost_python-py33'):
        envPython.Append(LIBS = 'boost_python-py33')
    elif conf.CheckLib('boost_python-py34'):
        envPython.Append(LIBS = 'boost_python-py34')
    else:
        envPython.Append(LIBS = 'boost_python3') 
else:
    envPython.ParseConfig("python-config --includes --ldflags")
    if conf.CheckLib('boost_python-py27'):
        envPython.Append(LIBS = 'boost_python-py27')
    else:
        envPython.Append(LIBS = 'boost_python') 

envPython = conf.Finish()

# versioned lib do not create correct links with .so in osx
if platform.system()=='Linux':
    sharedPyLib = envPython.SharedLibrary('lib/'+pythonLibraryName,  srcPyFiles, SHLIBVERSION=version)
    sharedLib = env.SharedLibrary('lib/'+libraryName, srcBaseFiles, SHLIBVERSION=version)
elif platform.system()=='Darwin':
    sharedPyLib = envPython.SharedLibrary('lib/'+pythonLibraryName,  srcPyFiles)
    sharedLib = env.SharedLibrary('lib/'+libraryName, srcBaseFiles)


# installation
installLibDir = env['installDir'] + '/lib/'
installHeadersDir = env['installDir'] + '/include/'
installAnalysisHeadersDir = installHeadersDir+'analysis'

installedLib = ""
installedPyLib = ""

if(LooseVersion(SCons.__version__) < LooseVersion("2.3.0")):
	installedLib = env.Install(installLibDir, sharedLib, SHLIBVERSION=version)
	installedPyLib = env.Install(installLibDir, sharedPyLib, SHLIBVERSION=version)
else:
	installedLib = env.InstallVersionedLib(installLibDir, sharedLib, SHLIBVERSION=version)
	installedPyLib = env.InstallVersionedLib(installLibDir, sharedPyLib, SHLIBVERSION=version)


installedHeaders = env.Install(installHeadersDir, coreHeaders)
installedAnalysisHeaders = env.Install(installAnalysisHeadersDir, analysisHeaders)

installBin = env.Install(env['installDir'], Glob('./bin'))
installShare = env.Install(env['installDir'], Glob('./share'))
installMpiStub = env.Install(env['installDir']+'/utils', Glob('./utils/*.cxx'))

# cassandra
cassandraCompilation = env.Command("bin/cassandra", "", "cd cassandra && qmake && make")
if platform.system()=='Darwin':
    cassandraCompilation = env.Command("bin/cassandra", "", "cd cassandra && qmake cassandra_osx.pro && make")

# final targets
Default(sharedLib)
Default(sharedPyLib)
env.Alias('cassandra', cassandraCompilation)
env.Alias('install', [cassandraCompilation, sharedLib, sharedPyLib, installedLib, installedPyLib, installedHeaders, installedAnalysisHeaders, installBin, installShare, installMpiStub])

