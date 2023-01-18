# Numerical-Recipes
All header files from numerical recipes in C++. 3rd Edition. 
Note that all these files are under copyright by their authors.
Check http://numerical.recipes/licenses/ for more information.

These files have been modified to actually compile out of the box (under Linux), 
unlike the many copies of these routines that can be found on Github but fail
to compile due to absence of the necessary includes.

This also means that the following dependencies have been added:
SuiteSparse AMD and LDL are included in the AMD and LDL directories, with the
corresponding libs built for Linux 64-bit.
Only the simplex.h file is missing the lp_solve library, which I could not compile.

https://github.com/DrTimothyAldenDavis/SuiteSparse

https://github.com/ESMG/PyCNAL_legacy/tree/master/bathy_smoother/external/lp_solve_5.5

No attempt has been made to make the code more modern or to use C++11 features.
The reason is because it makes it easier to compare the code with the original 
and the text in the book. I have passed the code through an indenter to make it easier
on the eye.
Barely any attempt has been made to clean compiler warnings nor correct mistakes
found by static analysis,  so the algorithms are pretty much verbatim.
They should compile on any modern C++ compiler with C++03 support.

**Use at your own risks !**

The code is not mine, and I am not responsible for any damage it may cause.
Most of it was written in the 80s and 90s. 
The code shows its age and its Fortran and C origins and would fail modern code quality
standards. The compiler will spit out a number of warnings and I only bothered to fix a
few of them.
Although I believe the results of these routines are correct (decades of bugs fixing),
I would not recommend using them without serious testing. But NR is a good 
introduction to numerical algorithms and a great starting point for educative purpose
(buy the book, it's totally worth it !).
For production work, I would recommend looking at modern libraries such as LAPACK, Intel 
MKL, Eigen, GNU Scientific Library, Ceres Solver, FFTW, etc. These are often much more 
efficient and reliable, written in more idiomatic C++, better tested and more actively 
maintained.

Finally, thereis only a few examples as a proof that the code does indeed
compile and run.
You will find many other examples in C here:
https://www.astro.umd.edu/~ricotti/NEWWEB/teaching/ASTR415/InClassExamples/NR3/legacy/nr2/CPP_211/progs.htm

== How to build ==
You will need cmake (adjust to your version in the first line of the CMakeLists.txt file, if necessary). 
Then go to the directory containing the CMakeLists.txt file (aka root of the project) and type:
cmake .
make