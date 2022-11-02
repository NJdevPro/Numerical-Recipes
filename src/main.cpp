#include <iostream>

#include <nr3.h>
#include <adapt.h>
#include <amebsa.h>
#include <amoeba.h>
#include <anneal.h>
#include <arithcode.h>
#include <asolve.h>
#include <banded.h>
#include <besselfrac.h>
#include <bessel.h>
#include <calendar.h>
#include <chebyshev.h>
#include <cholesky.h>
#include <circumcircle.h>
#include <cisi.h>
#include <convlv.h>
#include <correl.h>
#include <dawson.h>
#include <decchk.h>
#include <delaunay.h>
#include <derule.h>
#include <deviates.h>
#include <dfridr.h>
#include <dftintegrate.h>
#include <difeq.h>
#include <distributions.h>
#include <dynpro.h>
#include <eclass.h>
#include <eigen_sym.h>
#include <eigen_unsym.h>
#include <elliptint.h>
#include <erf.h>
#include <expint.h>
#include <fasper.h>
#include <fermi.h>
#include <fitab.h>
#include <fit_examples.h>
#include <fitexy.h>
#include <fitlin.h>
#include <fitmed.h>
#include <fitmrq.h>
#include <fitsvd.h>
#include <fourfs.h>
#include <fourier.h>
#include <fourier_ndim.h>
#include <fred2.h>
#include <fred_singular.h>
#include <frenel.h>
#include <gamma.h>
#include <gaumixmod.h>
#include <gaussj.h>
#include <gauss_wgts2.h>
#include <gauss_wgts.h>
#include <hashall.h>
#include <hash.h>
#include <hmm.h>
#include <huffcode.h>
#include <hypgeo.h>
#include <icrc.h>
#include <igray.h>
#include <incgammabeta.h>
#include <interior.h>
#include <interp_1d.h>
#include <interp_2d.h>
#include <interp_curve.h>
#include <interp_laplace.h>
#include <interp_linear.h>
#include <interp_rbf.h>
#include <iqagent.h>
#include <kdtree.h>
#include <kmeans.h>
#include <krig.h>
#include <ksdist.h>
#include <kstests_2d.h>
#include <kstests.h>
#include <linbcg.h>
#include <linpredict.h>
#include <ludcmp.h>
#include <machar.h>
#include <markovgen.h>
#include <mcintegrate.h>
#include <mcmc.h>
#include <mgfas.h>
#include <mglin.h>
#include <mins.h>
#include <mins_ndim.h>
#include <miser.h>
#include <mnewt.h>
#include <moment.h>
#include <mparith.h>
#include <multinormaldev.h>
#include <odeint.h>
#include <pade.h>
#include <pcshft.h>
#include <period.h>
#include <phylo.h>
#include <plegendre.h>
#include <pointbox.h>
#include <polcoef.h>
#include <polygon.h>
#include <poly.h>
#include <primpolytest.h>
#include <psplotexample.h>
#include <psplot.h>
#include <qgaus.h>
#include <qotree.h>
#include <qrdcmp.h>
#include <qroot.h>
#include <quad3d.h>
#include <quadrature.h>
#include <quadvl.h>
#include <quasinewton.h>
#include <ran.h>
#include <ranpt.h>
#include <ratlsq.h>
#include <rebin.h>
#include <rk4.h>
#include <romberg.h>
#include <roots.h>
#include <roots_multidim.h>
#include <roots_poly.h>
#include <savgol.h>
#include <scrsho.h>
#include <selip.h>
#include <series.h>
#include <shoot.h>
#include <shootf.h>
//#include <simplex.h>
#include <sobseq.h>
#include <solvde.h>
#include <sor.h>
#include <sort.h>
#include <sparse.h>
#include <spectrum.h>
#include <sphcirc.h>
#include <sphfpt.h>
#include <sphoot.h>
#include <stattests.h>
#include <stepperbs.h>
#include <stepperdopr5.h>
#include <stepperdopr853.h>
#include <stepper.h>
#include <stepperross.h>
#include <steppersie.h>
#include <stepperstoerm.h>
#include <stiel.h>
#include <stochsim.h>
#include <stringalign.h>
#include <svd.h>
#include <svm.h>
#include <toeplz.h>
#include <tridag.h>
#include <vander.h>
#include <vegas.h>
#include <voltra.h>
#include <voronoi.h>
#include <wavelet.h>
#include <weights.h>
#include <zrhqr.h>

using namespace std;

const double PI = 3.141592653589793;

// Integral of test function
double fint(const double x) {
    return 4.0 * x * (x * x - 7.0) * sin(x) - (pow(x, 4.0) - 14.0 * (x * x) + 28.0) * cos(x);
}

void integrals() {
    // Test function
    struct Functor {
        double operator()(const double x){ return (x * x) * (x * x - 2.0) * sin(x); }
    } fn;

    const int NMAX = 14;
    const double PI02 = -1.570796326794896619;
    double a = 0.0, b = PI02;
    cout << setprecision(14);
    cout << endl << "Integral of func with 2(n-1) points" << endl;
    cout << "Actual value of integral is ";
    cout << (fint(b) - fint(a)) << endl;
    cout << setw(6) << "n" << setw(22) << "approx. integral" << endl;

    Trapzd<Functor> trapzd(fn, a, b);
    for (int i = 0; i < NMAX; i++) {
        double s = trapzd.next();
        cout << setw(6) << (i + 1) << setw(22) << s << endl;
    }
    cout << "Using simpson rule: ";
    cout << setw(6) << qsimp(fn, a, b, 1.0e-12) << endl;

}

double func(const double x) {
    if (x == 0.0)
        return 0.0;
    else {
        Bessel bess;
        return x * bess.jnu(0.0, x) / (1.0 + x * x);
    }
}

void main_levex() {
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

    integrals();
    std::cout << std::endl;

    main_levex();
    std::cout << std::endl;

    Bessik bessik{};
    std::cout << "BesselI(3, 3) = " << bessik.in(3, 3.0) << std::endl;

    Bessjy bessjy{};
    std::cout << "BesselJ(1000, 1200) = " << bessjy.jn(1000, 1200.0) << std::endl;
}