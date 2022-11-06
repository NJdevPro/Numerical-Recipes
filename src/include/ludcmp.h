#pragma once
#include <nr3.h>

struct LUdcmp {
    size_t n;
    Doub d = 0.0;   // determinant
    MatDoub lu;     // lower upper matrix
    VecInt indx;    // permutation vector

    LUdcmp(MatDoub_I &a);

    void decompose(MatDoub &lower, MatDoub &upper);

    void solve(VecDoub_I &b, VecDoub_O &x);

    void solve(MatDoub_I &b, MatDoub_O &x);

    void inverse(MatDoub_O &ainv);

    Doub det();

    void mprove(VecDoub_I &b, VecDoub_IO &x);

    MatDoub_I &aref;    // ref to the original matrix
};


LUdcmp::LUdcmp(MatDoub_I &a) : n(a.nrows()), lu(a), aref(a), indx(n) {
    const Doub TINY = 1.0e-40;
    Doub big, temp;
    VecDoub vv(n);  // pivot coeffs for each row
    d = 1.0;        // sign
    // find the pivot for each row
    for (size_t i = 0; i < n; i++) {
        big = 0.0;
        for (size_t j = 0; j < n; j++)
            if ((temp = abs(lu[i][j])) > big) big = temp;
        if (big == 0.0) throw("Singular matrix in LUdcmp");
        vv[i] = 1.0 / big;
    }
    for (size_t k = 0; k < n; k++) {
        big = 0.0;
        size_t imax = k;
        for (size_t i = k; i < n; i++) {
            temp = vv[i] * abs(lu[i][k]);
            if (temp > big) {
                big = temp;
                imax = i;
            }
        }
        if (k != imax) {
            for (size_t j = 0; j < n; j++) {
                SWAP(lu[imax][j], lu[k][j]);
            }
            d = -d;
            vv[imax] = vv[k];
        }
        indx[k] = imax;
        if (lu[k][k] == 0.0) lu[k][k] = TINY;
        for (size_t i = k + 1; i < n; i++) {
            temp = lu[i][k] /= lu[k][k];
            for (size_t j = k + 1; j < n; j++)
                lu[i][j] -= temp * lu[k][j];
        }
    }
}

// Compose separately the lower and upper matrices from the LU decomposition
void LUdcmp::decompose(MatDoub &lower, MatDoub &upper) {
    for (size_t k = 0; k < n; k++) {
        for (size_t l = 0; l < n; l++) {
            if (l > k) {
                upper[k][l] = lu[k][l];
                lower[k][l] = 0.0;
            } else if (l < k) {
                upper[k][l] = 0.0;
                lower[k][l] = lu[k][l];
            } else {
                upper[k][l] = lu[k][l];
                lower[k][l] = 1.0;
            }
        }
    }
}

void LUdcmp::solve(VecDoub_I &b, VecDoub_O &x) {
    size_t ii = 0, ip;
    Doub sum = 0;
    if (b.size() != n || x.size() != n) throw("LUdcmp::solve bad sizes");
    for (size_t i = 0; i < n; i++) x[i] = b[i];
    for (size_t i = 0; i < n; i++) {
        ip = indx[i];
        sum = x[ip];
        x[ip] = x[i];
        if (ii != 0)
            for (size_t j = ii - 1; j < i; j++) sum -= lu[i][j] * x[j];
        else if (sum != 0.0)
            ii = i + 1;
        x[i] = sum;
    }
    for (size_t i = n; i-- > 0;) {
        sum = x[i];
        for (size_t j = i + 1; j < n; j++) sum -= lu[i][j] * x[j];
        x[i] = sum / lu[i][i];
    }
}

void LUdcmp::solve(MatDoub_I &b, MatDoub_O &x) {
    size_t m = b.ncols();
    if (b.nrows() != n || x.nrows() != n || b.ncols() != x.ncols()) throw("LUdcmp::solve bad sizes");
    VecDoub xx(n);
    for (size_t j = 0; j < m; j++) {
        for (size_t i = 0; i < n; i++) xx[i] = b[i][j];
        solve(xx, xx);
        for (size_t i = 0; i < n; i++) x[i][j] = xx[i];
    }
}

void LUdcmp::inverse(MatDoub_O &ainv) {
     ainv.resize(n, n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) ainv[i][j] = 0.;
        ainv[i][i] = 1.;
    }
    solve(ainv, ainv);
}

Doub LUdcmp::det() {
    Doub dd = d;
    for (size_t i = 0; i < n; i++) dd *= lu[i][i];
    return dd;
}

void LUdcmp::mprove(VecDoub_I &b, VecDoub_IO &x) {
    VecDoub r(n);
    for (size_t i = 0; i < n; i++) {
        Ldoub sdp = -b[i];
        for (size_t j = 0; j < n; j++)
            sdp += (Ldoub) aref[i][j] * (Ldoub) x[j];
        r[i] = sdp;
    }
    solve(r, r);
    for (size_t i = 0; i < n; i++) x[i] -= r[i];
}
