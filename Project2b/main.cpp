/*
 * Valarray_PhaseA_unittests.cpp
 * EPL - Spring 2015
 */

#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>

#include "Valarray.h"

using std::cout;
using std::endl;
using std::string;
using std::complex;

using namespace epl;

int main(void){
    valarray<complex<double>> x(10);
    for(int i = 0; i < 10; ++i){
        x[i] = complex<double>(i, i);
    }

    //passes
    auto y = x + x;
    cout << y << endl;
}
