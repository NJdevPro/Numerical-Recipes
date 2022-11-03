#include <iostream>
#include <iomanip>
#include "nr3.h"
#include "ran.h"

#include "anneal.h"

using namespace std;

// Driver for routine anneal
// Type your code here, or load an example.
#include <algorithm>
#include <iostream>
#include <chrono>
#include <ctime>
#include <ratio>


void clock(void (*func)(void))
{
    using namespace std::chrono;

    typedef std::chrono::high_resolution_clock Clock;

    auto t1 = Clock::now();
    func();
    auto t2 = Clock::now();

    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << std::fixed << std::setprecision(5) <<"It took me " <<  time_span.count() << " seconds.";
    std::cout << std::endl;
}

void travelling_saleseman()
{
    const int NCITY = 30;
    int idum = (-111);
    VecInt iorder(NCITY);
    VecDoub x(NCITY), y(NCITY);
    Ran ran(idum);

    for (int i = 0; i < NCITY; i++) {
        x[i] = ran.int32()/100000;
        y[i] = ran.int32()/100000;
        iorder[i] = i;
    }

    Anneal anneal;
    cout << endl << "Travelling salesman problem" << endl;
    anneal.order(x, y, iorder);
    cout << endl << "*** System Frozen ***" << endl;
    cout << "Final path:" << endl;
    cout << setw(5) << "city" << setw(8) << "x" << setw(11) << "y" << endl;
    cout << fixed << setprecision(1);
    for (int i = 0; i < NCITY; i++) {
        int ii = iorder[i];
        cout << setw(4) << ii << setw(11) << x[ii];
        cout << setw(11) << y[ii] << endl;
    }
}

int main(void) {
    clock(&travelling_saleseman);
    return 0;
}