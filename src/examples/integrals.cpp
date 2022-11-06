//
// Created by njanin on 02/11/22.
//

#include <iostream>
#include "quadrature.h"
#include "romberg.h"

using namespace std;

// Test function
struct Functor {
    double operator()(const double x){ return (x * x) * (x * x - 2.0) * sin(x); }
} fn;

// Integral of examples function
double fint(const double x) {
    return 4.0 * x * (x * x - 7.0) * sin(x) - (pow(x, 4.0) - 14.0 * (x * x) + 28.0) * cos(x);
}

void integrals() {
    const int NMAX = 20;
    const double PI02 = -1.570796326794896619;
    double a = 0.0, b = PI02;
    cout << setprecision(16);
    cout << endl << "Integral of func x² * (x² - 2.0) * sin(x) with 2(n-1) points" << endl;
    cout << "Actual value of integral is " << (fint(b) - fint(a)) << endl;
    cout << "Using trapezoidal rule:" << endl;
    cout << setw(6) << "n" << setw(22) << "approx. integral" << endl;

    Trapzd<Functor> trapzd(fn, a, b);
    for (int i = 0; i < NMAX; i++) {
        double s = trapzd.next();
        cout << setw(6) << (i + 1) << setw(22) << s << endl;
    }
    cout << "Using Simpson rule: ";
    cout << setw(6) << qsimp(fn, a, b, 1.0e-12) << endl;
    cout << "Using Romberg rule: ";
    cout << setw(6) << qromb(fn, a, b, 1.0e-14) << endl;

}

int main(int argc, char **argv) {
    integrals();
}