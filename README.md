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
and the text in the book.
Barely any attempt has been made to clean compiler warnings nor correct mistakes
found by static analysis,  so the algorithms are pretty much verbatim.
They should compile on any modern C++ compiler with C++03 support.

Use at your own risks !

The code is not mine, and I am not responsible for any damage it may cause.
Most of it was written in the 80s, has not really been updated since, and is not 
meant to be used in production. The code shows its age and its Fortran origins.
I would not recommend using these routines for anything serious, but NR is a good 
introduction to numerical algorithms and a great starting point for educative purpose
(buy the book !).
For production work, I would recommend looking at modern libraries such as MKL, Eigen, 
GNU Scientific Library, Ceres Solver, FFTW, etc. These are often much more efficient and
reliable, written in more idiomatic C++, better tested and more actively maintained.

Finally, I could not find a copy of the example programs that were shipped along the 
book of examples. If you have a copy, it would be great to add them to this repository.