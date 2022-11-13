#include <iostream>
#include <romberg.h>
#include "series.h"
#include "besselfrac.h"


const double PI = 3.141592653589793;

double func(const double x) {
    if (x == 0.0)
        return 0.0;
    else {
        Bessel bess;
        return x * bess.jnu(0.0, x) / (1.0 + x * x);
    }
}

void levex() {
    int nterm = 12;
    double beta = 1.0, a = 0.0, b = 0.0, sum = 0.0;
    Levin series(100, 0.0);
    std::cout << std::setw(5) << "N" << std::setw(19) << "Sum (direct)" << std::setw(21)
              << "Sum (Levin)" << std::endl;
    for (int n = 0; n <= nterm; n++) {
        b += PI;
        double s = qromb(func, a, b, 1.e-8);
        a = b;
        sum += s;


        double omega = (beta + n) * s;
        double ans = series.next(sum, omega, beta);
        std::cout << std::setw(5) << n << std::fixed << std::setprecision(14) << std::setw(21)
                  << sum << std::setw(21) << ans << std::endl;
    }
}

int main(int argc, char **argv) {

    std::cout << std::setprecision(12);
    levex();
}

