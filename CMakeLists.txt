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
add_executable(levex src/examples/levex.cpp)
add_executable(ludcmp src/examples/ludcomp.cpp)
add_executable(odeint src/examples/odeint.cpp)
add_executable(sfroid src/examples/sfroid.cpp)
add_executable(vegas src/examples/vegas.cpp)

add_test(NAME anneal_example COMMAND anneal)
add_test(NAME bessel_example COMMAND bessel)
add_test(NAME integrals_example COMMAND integrals)
add_test(NAME levex_example COMMAND levex)
add_test(NAME ludcmp_example COMMAND ludcmp)
add_test(NAME sfroid_example COMMAND sfroid)
add_test(NAME vegas_example COMMAND vegas)
