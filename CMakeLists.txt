cmake_minimum_required(VERSION 3.23)
project(Numerical_Recipes)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wno-reorder")

include_directories(src/include)
link_directories(lib)
link_libraries(libamd.a libldl.a libsuitesparseconfig.a)

add_executable(Numerical_Recipes src/main.cpp)

add_executable(anneal src/test/anneal.cpp)
add_executable(bessel src/test/bessel.cpp)
add_executable(integrals src/test/integrals.cpp)
add_executable(jacobi src/test/jacobi.cpp)
add_executable(levex src/test/levex.cpp)
add_executable(odeint src/test/odeint.cpp)
