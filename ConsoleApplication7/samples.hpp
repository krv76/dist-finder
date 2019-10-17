// Copyright 2019 Roman Kukushkin

#pragma once

#include "curve2d.hpp"
#include <fstream>
#include <string>

namespace samples {

void CreateBezie1(const std::string& fileName, unsigned seed, double left, double bottom, double right, double top, double amp, size_t n);

void OpenReadBezieAndClose(const std::string& fileName, BezieSpline& bs);
}  // namespace samples
