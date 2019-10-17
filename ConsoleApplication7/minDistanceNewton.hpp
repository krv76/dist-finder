// Copyright 2019 Roman Kukushkin

#pragma once
#include "curve2d.hpp"
#include <vector>

// ��������� ������ ��������� ��� ���������� ��� ���� ������
std::vector<ParamPair> getClosestPointsPairs(const Curve2d& curve1, const Curve2d& curve2);
