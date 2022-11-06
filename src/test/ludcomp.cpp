#include <string>
#include <iostream>
#include <iomanip>
#include "nr3.h"
#include "ludcmp.h"

using namespace std;

MatDoub prod(const MatDoub &xl, const MatDoub &xu) {
    if (xl.nrows() != xu.ncols()) throw("Bad matrix sizes");
    size_t n = xl.nrows();
    MatDoub x(n, n);
    for (size_t k = 0; k < n; k++) {
        for (size_t l = 0; l < n; l++) {
            x[k][l] = 0.0;
            for (size_t j = 0; j < n; j++)
                x[k][l] += (xl[k][j] * xu[j][l]);
        }
    }
    return x;
}

void print_M(const MatDoub &x) {
    size_t n = x.nrows();
    for (size_t k = 0; k < n; k++) {
        for (size_t l = 0; l < n; l++) cout << setw(12) << x[k][l];
        cout << endl;
    }
}

void print_V(const NRvector<double> x) {
    for (size_t l = 0; l < x.size(); l++) cout << setw(12) << x[l];
    cout << endl;
}

int main(void) {
    ifstream fp("/home/njanin/CLionProjects/Numerical-Recipes/src/test/matri1.dat");
    if (fp.fail()) throw ("Data file matrx1.dat not found");
    cout << fixed << setprecision(6);
    string txt;
    getline(fp, txt);

    while (!fp.eof()) {
        size_t m, n;
        getline(fp, txt);
        fp >> n >> m;   // matrix size
        getline(fp, txt);
        MatDoub a(n, n), // original matrix
        xl(n, n),    // lower triangular matrix
        xu(n, n),    // upper triangular matrix
        x(n, n);     // product of xl and xu
        getline(fp, txt);
        for (size_t k = 0; k < n; k++)
            for (size_t l = 0; l < n; l++) fp >> a[k][l];
        getline(fp, txt);
        getline(fp, txt);
        for (size_t l = 0; l < m; l++)
            for (size_t k = 0; k < n; k++) fp >> x[k][l];
        getline(fp, txt);
        getline(fp, txt);
        // Print out a-matrix for comparison with product of
        // lower and upper decomposition matrices
        cout << "Original matrix A =" << endl;
        print_M(a);
        // Perform the LU decomposition
        LUdcmp ludcmp(a);
        ludcmp.decompose(xl, xu);

        // Compute product of lower and upper matrices for
        // comparison with original matrix
        x = prod(xl, xu);
        cout << endl << "Product L.U = " << endl;
        VecInt jndx(n); // permutation vector
        for (size_t k = 0; k < n; k++) jndx[k] = k; // jndx[i] = i
        for (size_t k = 0; k < n; k++) {
            SWAP(jndx[ludcmp.indx[k]], jndx[k]);
        }
        for (size_t k = 0; k < n; k++)
            for (size_t j = 0; j < n; j++)
                if (jndx[j] == k) {
                    for (size_t l = 0; l < n; l++) cout << setw(12) << x[j][l];
                    cout << endl;
                }
        cout << endl << "Lower matrix L =" << endl;
        print_M(xl);
        cout << endl << "Upper matrix U =" << endl;
        print_M(xu);
        cout << "det = " << ludcmp.det() << endl;
        cout << "Inverse of A= \n";
        NRmatrix<double> a_orig = ludcmp.aref;
        ludcmp.inverse(a);
        print_M(a);
        cout << "Product  A.inv(A) = \n";
        x = prod(a, a_orig);
        print_M(x);
        cout << endl << "***********************************" << endl;
        cout << "NEXT PROBLEM" << endl;
    }
    fp.close();
    return 0;
}
