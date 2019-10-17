// Copyright 2019 Roman Kukushkin

#include "stdafx.h"
#include "samples.hpp"
#include "point2d.hpp"

#include "cstdlib"
#include <iomanip>
#include <string>

namespace {

Point2d GetRandomPoint(double left, double bottom, double right, double top) {
    const size_t RANDOM_STEP = 1000;
    return Point2d(
        left + (right - left) * (rand() % RANDOM_STEP) / RANDOM_STEP,
        bottom + (top - bottom) * (rand() % RANDOM_STEP) / RANDOM_STEP
    );
}

void CreateBezie1(std::ofstream & ofs, unsigned seed, double left, double bottom, double right, double top, double amp, size_t n) {
    srand(seed);
    ofs << n << "\n";
    ofs << std::setprecision(15);
    Point2d node = GetRandomPoint(left, bottom, left, top);
    ofs << node << "\n";
    Point2d delta = GetRandomPoint(0, -amp, amp, 0);
    ofs << node + delta << "\n";
    for (size_t i = 1; i < n; ++i) {
        node = GetRandomPoint(left + i * (right - left) / n, bottom, left + i * (right - left) / n, top);
        delta = GetRandomPoint(-amp, -amp, amp, amp);
        ofs << node - delta << "\n";
        ofs << node << "\n";
        delta = GetRandomPoint(-amp, -amp, amp, amp);
        ofs << node + delta << "\n";
    }
    node = GetRandomPoint(right, bottom, right, top);
    delta = GetRandomPoint(0, -amp, amp, 0);
    ofs << node - delta << "\n";
    ofs << node << "\n";
}
}  // namespace

// —оздание и вывод в поток случайной кривой Ѕезье с заданными границами и заданным числом контрольных точек
namespace samples {

void OpenReadBezieAndClose(const std::string & fileName, BezieSpline & bs) {
    std::ifstream ifs(fileName);
    bs.readFromFStream(ifs);
    ifs.close();
}

void CreateBezie1(const std::string & fileName, unsigned seed, double left, double bottom, double right, double top, double amp, size_t n) {
    std::ofstream ofs(fileName);
    ::CreateBezie1(ofs, seed, left, bottom, right, top, amp, n);
}
}  // namespace samples
