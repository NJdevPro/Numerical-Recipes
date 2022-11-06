cmake_minimum_required(VERSION 3.23)
project(Numerical_Recipes)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wno-reorder")

include_directories(src/include)
link_directories(lib)
link_libraries(libamd.a libldl.a libsuitesparseconfig.a)

add_executable(Numerical_Recipes src/main.cpp)

add_executable(anneal src/examples/anneal.cpp)
add_executable(bessel src/examples/bessel.cpp)
add_executable(integrals src/examples/integrals.cpp)
add_executable(ludcmp src/examples/ludcomp.cpp)
add_executable(levex src/examples/levex.cpp)
add_executable(odeint src/examples/odeint.cpp)
add_executable(sfroid src/examples/sfroid.cpp)
