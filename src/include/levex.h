#pragma once
#include <nr3.h>

#include "series.h"
#include "romberg.h"

Doub func(const Doub x) {
    if (x == 0.0)
        return 0.0;
    else {
        Bessel bess;
        return x * bess.jnu(0.0, x) / (1.0 + x * x);
    }
}

Int main_levex(void) {
    const Doub PI = 3.141592653589793;
    Int nterm = 12;
    Doub beta = 1.0, a = 0.0, b = 0.0, sum = 0.0;
    Levin series(100, 0.0);
    std::cout << std::setw(5) << "N" << std::setw(19) << "Sum (direct)" << std::setw(21)
         << "Sum (Levin)" << std::endl;
    for (Int n = 0; n <= nterm; n++) {
        b += PI;
        Doub s = qromb(func, a, b, 1.e-8);
        a = b;
        sum += s;
        Doub omega = (beta + n) * s;
        Doub ans = series.next(sum, omega, beta);
        std::cout << std::setw(5) << n << std::fixed << std::setprecision(14) << std::setw(21)
             << sum << std::setw(21) << ans << std::endl;
    }
    return 0;
}
