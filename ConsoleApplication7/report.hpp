// Copyright 2019 Roman Kukushkin

#pragma once

#include "stdafx.h"
#include "misc.hpp"
#include "minDistanceNewton.hpp"
#include <string>

// TODO �������� ����������� ��������� �������

// ��������� � svg �������, ��������� ������� � ��������� � �������� className
void SvgDrawEllipse(std::ofstream& ofs, const Point2d &c, const Point2d &r,
    const std::string& className);

// ��������� ���������� � ��������� ����� � �����
// <nameWithNoExtension>.svg
// <nameWithNoExtension>.txt
void SolveAndMakeReports(const Curve2d & curve1, const Curve2d & curve2,
    const std::string nameWithNoExtension);
