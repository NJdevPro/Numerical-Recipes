#pragma once
#include <nr3.h>

struct Machar {
    Int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
    Doub eps, epsneg, xmin, xmax;

    Machar() {
        Int i, itemp, iz, j, k, mx, nxres;
        Doub a, b, beta, betah, betain, one, t, temp, temp1, tempa, two, y, z, zero;
        one = Doub(1);
        two = one + one;
        zero = one - one;
        a = one;
        do {
            a += a;
            temp = a + one;
            temp1 = temp - a;
        } while (temp1 - one == zero);
        b = one;
        do {
            b += b;
            temp = a + b;
            itemp = Int(temp - a);
        } while (itemp == 0);
        ibeta = itemp;
        beta = Doub(ibeta);
        it = 0;
        b = one;
        do {
            ++it;
            b *= beta;
            temp = b + one;
            temp1 = temp - b;
        } while (temp1 - one == zero);
        irnd = 0;
        betah = beta / two;
        temp = a + betah;
        if (temp - a != zero) irnd = 1;
        tempa = a + beta;
        temp = tempa + betah;
        if (irnd == 0 && temp - tempa != zero) irnd = 2;
        negep = it + 3;
        betain = one / beta;
        a = one;
        for (i = 1; i <= negep; i++) a *= betain;
        b = a;
        for (;;) {
            temp = one - a;
            if (temp - one != zero) break;
            a *= beta;
            --negep;
        }
        negep = -negep;
        epsneg = a;
        machep = -it - 3;
        a = b;
        for (;;) {
            temp = one + a;
            if (temp - one != zero) break;
            a *= beta;
            ++machep;
        }
        eps = a;
        ngrd = 0;
        temp = one + eps;
        if (irnd == 0 && temp * one - one != zero) ngrd = 1;
        i = 0;
        k = 1;
        z = betain;
        t = one + eps;
        nxres = 0;
        for (;;) {
            y = z;
            z = y * y;
            a = z * one;
            temp = z * t;
            if (a + a == zero || abs(z) >= y) break;
            temp1 = temp * betain;
            if (temp1 * beta == z) break;
            ++i;
            k += k;
        }
        if (ibeta != 10) {
            iexp = i + 1;
            mx = k + k;
        } else {
            iexp = 2;
            iz = ibeta;
            while (k >= iz) {
                iz *= ibeta;
                ++iexp;
            }
            mx = iz + iz - 1;
        }
        for (;;) {
            xmin = y;
            y *= betain;
            a = y * one;
            temp = y * t;
            if (a + a != zero && abs(y) < xmin) {
                ++k;
                temp1 = temp * betain;
                if (temp1 * beta == y && temp != y) {
                    nxres = 3;
                    xmin = y;
                    break;
                }
            } else break;
        }
        minexp = -k;
        if (mx <= k + k - 3 && ibeta != 10) {
            mx += mx;
            ++iexp;
        }
        maxexp = mx + minexp;
        irnd += nxres;
        if (irnd >= 2) maxexp -= 2;
        i = maxexp + minexp;
        if (ibeta == 2 && !i) --maxexp;
        if (i > 20) --maxexp;
        if (a != y) maxexp -= 2;
        xmax = one - epsneg;
        if (xmax * one != xmax) xmax = one - beta * epsneg;
        xmax /= (xmin * beta * beta * beta);
        i = maxexp + minexp + 3;
        for (j = 1; j <= i; j++) {
            if (ibeta == 2) xmax += xmax;
            else xmax *= beta;
        }
    }

    void report() {
        std::cout << "quantity:  std::numeric_limits<Doub> says  (we calculate)" << std::endl;
        std::cout << "radix:  " << std::numeric_limits<Doub>::radix
             << "  (" << ibeta << ")" << std::endl;
        std::cout << "mantissa digits:  " << std::numeric_limits<Doub>::digits
             << "  (" << it << ")" << std::endl;
        std::cout << "round style:  " << std::numeric_limits<Doub>::round_style
             << "  (" << irnd << ") [our 5 == IEEE 1]" << std::endl;
        std::cout << "guard digits:  " << "[not in std::numeric_limits]"
             << "  (" << ngrd << ")" << std::endl;
        std::cout << "epsilon:  " << std::numeric_limits<Doub>::epsilon()
             << "  (" << eps << ")" << std::endl;
        std::cout << "neg epsilon:  " << "[not in std::numeric_limits]"
             << "  (" << epsneg << ")" << std::endl;
        std::cout << "epsilon power:  " << "[not in std::numeric_limits]"
             << "  (" << machep << ")" << std::endl;
        std::cout << "neg epsilon power:  " << "[not in std::numeric_limits]"
             << "  (" << negep << ")" << std::endl;
        std::cout << "exponent digits:  " << "[not in std::numeric_limits]"
             << "  (" << iexp << ")" << std::endl;
        std::cout << "min exponent:  " << std::numeric_limits<Doub>::min_exponent
             << "  (" << minexp << ")" << std::endl;
        std::cout << "max exponent:  " << std::numeric_limits<Doub>::max_exponent
             << "  (" << maxexp << ")" << std::endl;
        std::cout << "minimum:  " << std::numeric_limits<Doub>::min()
             << "  (" << xmin << ")" << std::endl;
        std::cout << "maximum:  " << std::numeric_limits<Doub>::max()
             << "  (" << xmax << ")" << std::endl;
    }
};
