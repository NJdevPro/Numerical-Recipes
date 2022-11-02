//
// Created by njanin on 02/11/22.
//

#include <iostream>
#include <nr3.h>
#include "odeint.h"
#include "stepperdopr5.h"
#include "stepperdopr853.h"
#include "stepperbs.h"
#include "rk4.h"
#include "bessel.h"
#include "besselfrac.h"

// Van der Pol oscillator
struct rhs_van {
    double eps;

    rhs_van(Doub epss) : eps(epss) {}

    void operator()(const Doub x, VecDoub_I &y, VecDoub_O &dydx) {
        dydx[0] = y[1];
        dydx[1] = ((1.0 - y[0] * y[0]) * y[1] - y[0]) / eps;
    }
};

template <class Stepper>
void odeint()
{
    const double atol = 1.0e-6, rtol = atol, h1 = 1.0e-5, hmin = 0.0;
    const double x1 = 0.0, x2 = 2.0;
    VecDoub ystart(2);
    ystart[0] = 2.0;
    ystart[1] = 0.0;

    Output out(30);         // number of points to save
    rhs_van d(1.0e-3);      // Van der Pol oscillator with eps=1.0e-3
    Odeint<Stepper> ode(ystart, x1, x2, atol, rtol, h1, hmin, out, d);
    ode.integrate();

    std::cout << std::setprecision(12);
    std::cout << std::endl << "Solution of y_1' = [(1-y_0²)y_1 - y_0]/epsilon with y_0' = y_1" << std::endl;
    for (int i = 0; i < out.count; i++)
        std::cout << std::setw(3) << i << " "
                  << std::setw(15) << out.xsave[i] << " "
                  << std::setw(15) << out.ysave[0][i] << " "
                  << std::setw(15) << out.ysave[1][i]
                  << std::endl;
}

void derivs(const double x, VecDoub_I &y, VecDoub_O &dydx)
{
    dydx[0]= -y[1];
    dydx[1]=y[0]-(1.0/x)*y[1];
    dydx[2]=y[1]-(2.0/x)*y[2];
    dydx[3]=y[2]-(3.0/x)*y[3];
}

void rk4int() {
    const int N = 4;
    double h, x = 1.0;
    VecDoub y(N), dydx(N), yout(N);

    Bessjy bessjy;
    y[0] = bessjy.j0(x);
    y[1] = bessjy.j1(x);
    y[2] = bessjy.jn(2, x);
    y[3] = bessjy.jn(3, x);
    derivs(x, y, dydx);
    std::cout << std::endl << std::setw(16) << "Bessel function:";
    std::cout << std::setw(6) << "j0" << std::setw(13) << "j1";
    std::cout << std::setw(13) << "j3" << std::setw(13) << "j4" << std::endl;
    std::cout << std::fixed;

    for (int i = 0; i < 5; i++) {
        h = 0.2 * (i + 1);
        rk4(y, dydx, x, h, yout, derivs);
        std::cout << std::setprecision(2);
        std::cout << std::endl << "for a step size of: " << std::setw(6) << h << std::endl;
        std::cout << std::setprecision(6);
        std::cout << std::setw(12) << "rk4:";
        for (int j = 0; j < 4; j++) std::cout << std::setw(13) << yout[j];
        std::cout << std::endl << std::setw(12) << "actual:" << std::setw(13) << bessjy.j0(x + h);
        std::cout << std::setw(13) << bessjy.j1(x + h) << std::setw(13) << bessjy.jn(2, x + h);
        std::cout << std::setw(13) << bessjy.jn(3, x + h) << std::endl;
    }
}

int main(int argc, char **argv) {

    std::cout << std::setprecision(10);

    odeint<StepperDopr5<rhs_van>>();
    odeint<StepperDopr853<rhs_van>>();
    odeint<StepperBS<rhs_van>>();

    rk4int();
}

