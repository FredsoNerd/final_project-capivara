cmake_minimum_required(VERSION 3.0)
find_package(PythonLibs 3 REQUIRED)
#To prevent attempts to link to python2 libraries, specify
#the version of libboost_python
#See https://gitlab.kitware.com/cmake/cmake/issues/16391

find_package(Boost QUIET)
find_package(Boost REQUIRED python)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

python_add_module(structures capivara/structures.cpp)

target_include_directories(structures PUBLIC
  ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
target_link_libraries(structures
  ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})