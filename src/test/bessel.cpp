//
// Created by njanin on 02/11/22.
//
#include <iostream>
#include <bessel.h>

void bessel()
{
    Bessik bessik{};
    std::cout << "BesselI(3, 3) = " << bessik.in(3, 3.0) << std::endl;
    std::cout << "BesselK(3, 3) = " << bessik.kn(3, 3.0) << std::endl;
    Bessjy bessjy{};
    std::cout << "BesselJ(1000, 1200) = " << bessjy.jn(1000, 1200.0) << std::endl;
    std::cout << "BesselY(1000, 1200) = " << bessjy.yn(1000, 1200.0) << std::endl;
}

int main(int argc, char **argv) {

    std::cout << std::setprecision(12);
    bessel();
}