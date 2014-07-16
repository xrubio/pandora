cmake_minimum_required (VERSION 2.6)
project (pandora)

# set compilers
SET(CMAKE_CXX_COMPILER mpicxx)

# set build type
# set(CMAKE_BUILD_TYPE Release)

# set vars
set(HDF5PATH "/usr/local/hdf5")
set(BOOSTPATH "/usr/local/boost_1_55_0")

# define c++ flags
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++0x -g -Wall -DTIXML_USE_STL -fopenmp -DPANDORADEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DTIXML_USE_STL -fopenmp -std=c++0x")

# set prefix for debug compiling mode
set(CMAKE_DEBUG_POSTFIX "d")

# include folder
include_directories(. analysis/ ${BOOSTPATH}/include /usr/include ${HDF5PATH}/include /usr/include/gdal)

# path folders
LINK_DIRECTORIES(/usr/local/lib ${HDF5PATH}/lib ${BOOSTPATH}/lib)

# search for all files to compile
file(GLOB sourcefiles_SRC
    "*.cxx"
    "analysis/*.cxx"
)
# exclude file
list(REMOVE_ITEM sourcefiles_SRC "MpiStubCode.cxx")

# define lib
add_library(pandora  SHARED ${sourcefiles_SRC})

# link libraries
target_link_libraries(pandora pthread gdal hdf5 z tinyxml boost_filesystem boost_system boost_timer boost_chrono gomp mpl dl) 

# -----------------------------------------------------------------------------------------------
#       DEFINE PYLIBPANDORA
# -----------------------------------------------------------------------------------------------

# define c++ flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -DTIXML_USE_STL  -std=c++0x -DPANDORADEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DTIXML_USE_STL -std=c++0x")

# search for all files to compile
file(GLOB pysourcefiles_SRC    
    "*.cxx"
    "analysis/*.cxx"
    "pyPandora/pyBindings.cxx"
    "MpiStubCode.cxx"
)
include_directories(/usr/include/python3.2/)

# define lib
add_library(pyPandora SHARED ${pysourcefiles_SRC})

target_link_libraries(pyPandora pthread gdal hdf5 z tinyxml boost_filesystem boost_system boost_timer boost_chrono rt gomp mpl dl boost_python-py32 python3.2mu)

