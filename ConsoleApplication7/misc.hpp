// Copyright 2019 Roman Kukushkin

#pragma once
#include "point2d.hpp"
#include "curve2d.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

const double M_PI = 3.1415926535897932384626433832795;

template<typename T>
T bound(const T& x, const T& xMin, const T& xMax) {
    return std::min(std::max(x, xMin), xMax);
}

template<typename T>
bool inBounds(const T& x, const T& xMin, const T& xMax) {
    return x >= xMin && x <= xMax;
}

struct ParamPair {
    double tFirst, tSecond;
};
