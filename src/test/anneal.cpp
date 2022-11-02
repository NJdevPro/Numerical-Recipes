#include <iostream>
#include <iomanip>
#include "nr3.h"
#include "ran.h"

#include "anneal.h"

using namespace std;

// Driver for routine anneal

int main(void) {
    const int NCITY = 15;
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
    return 0;
}