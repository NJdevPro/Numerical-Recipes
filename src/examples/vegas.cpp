#include <iostream>
#include <iomanip>
#include <cmath>
#include "nr3.h"
#include "vegas.h"

using namespace std;

int ndim;       // for fxn
double xoff;

double fxn(VecDoub_I &pt, const double wgt) {
    int j;
    double ans, sum;

    for (sum = 0.0, j = 0; j < ndim; j++) sum += (100.0 * SQR(pt[j] - xoff));
    ans = (sum < 80.0 ? exp(-sum) : 0.0);
    ans *= pow(5.64189, double(ndim));
    return ans;
}

int main(void) {
    int init, itmax, j, ncall, nprn;
    double avgi, chi2a, sd, xoff;

    cout << fixed << setprecision(6);
    // You can try: 3 0 10000 5 1
    cout << "Enter NDIM XOFF NCALL ITMAX NPRN (NDIM=0 to stop)" << endl;
    //cin >> ndim >> xoff >> ncall >> itmax >> nprn;
    ndim = 3;
    xoff = 0;
    ncall = 10000;
    itmax = 5;
    nprn = 1;
    VecDoub regn(2 * ndim);
    avgi = sd = chi2a = 0.0;
    for (j = 0; j < ndim; j++) {
        regn[j] = 0.0;
        regn[j + ndim] = 1.0;
    }
    init = -1;
    vegas(regn, fxn, init, ncall, itmax, nprn, avgi, sd, chi2a);
    cout << "Number of iterations performed: " << itmax << endl;
    cout << "Integral, Standard Dev., Chi-sq. = ";
    cout << setw(13) << avgi << setw(13) << sd;
    cout << setw(13) << chi2a << endl;
    init = 1;
    vegas(regn, fxn, init, ncall, itmax, nprn, avgi, sd, chi2a);
    cout << "Additional iterations performed: " << itmax << endl;
    cout << "Integral, Standard Dev., Chi-sq. = ";
    cout << setw(13) << avgi << setw(13) << sd;
    cout << setw(13) << chi2a << endl << endl;
    cout << "Normal completion" << endl;
    return 0;
}