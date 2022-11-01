cmake_minimum_required(VERSION 3.23)
project(Numerical_Recipes)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wno-reorder")

include_directories(src/include)
link_directories(lib)
link_libraries(libamd.a libldl.a libsuitesparseconfig.a)

add_executable(Numerical_Recipes src/main.cpp)

#add_executable(sfroid src/sfroid.cpp)
