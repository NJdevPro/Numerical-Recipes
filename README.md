# Numerical-Recipes
All header files from numerical recipes in C++. 3rd Edition. 
Note that all these files are under copyright of their authors.
Check http://numerical.recipes/licenses/ for more information.

These files have been modified to actually compile out of the box (under Linux), 
unlike the many copies of these routines that can be found on Github but fail
to compile due to absence of the necessary includes.
This also means that the following dependencies have been added:
SparseSuite AMD and LDL are included in the AMD and LDL directories, with the
corresponding libs built for Linux 64-bit.
Only the simplex.h file is missing the lp_solve library, which I could not compile.

No attempt has been made to make the code more modern or to use C++11 features.
This should compile on any modern C++ compiler with C++03 support.
The reason is because it makes it easier to compare the code with the original 
and the text in the book.
Barely any attempt has been made to clean compiler warnings nor correct mistakes
found by static analysis,  so the algorithms are pretty much verbatim.

Use at your own risks !

