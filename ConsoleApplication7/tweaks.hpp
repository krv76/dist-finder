// Copyright 2019 Roman Kukushkin

#pragma once

#include "stdafx.h"
#include <string>

// ��������� ������������ ��� ��������� ���������� �����������
const double DERIVATIVE_DELTA = 1e-6;

// ���� �� �������� ���� ��������, �������, ��� ���������� ����������� � ������������ �������������� ��� ����������� ��������
const double INTERSECT_PROCESSING_BOUND = 0.001;

// ��������, ������� ���������� ��������
const double DESIRED_PRECISION = 1e-9;
const double DESIRED_PRECISION2 = DESIRED_PRECISION * DESIRED_PRECISION;

// ������� ����� ����� ����������� ��� "����� �������"
const double CLOSE_POINTS = 1e-4;
const double CLOSE_POINTS2 = CLOSE_POINTS * CLOSE_POINTS;

// ��������� ���������� ��������� -
// �������� ��������, ��� ������� ���������� ������ �� ����������
const double SHARPENING_COS_MAX = 0.90;
// ����� �����, �� ������� �� ����� ����������, ����� ��������,
// �������� �� �� "����� ������"
const size_t SHARPENING_N = 20;

// ����� ��������������� ��������
const size_t DRAFT_ITERATIONS_NUMBER = 5;

// ����� ���������� ��������
const size_t FINE_ITERATIONS_NUMBER = 15;

// ���� ����� ��������������� �������� ���������
// DRAFT_MUL * (best_dist) + DRAFT_DELTA > current_dist,
// ����� ����������� � ������� ��� ��� ��������� ��� ���� ���������� ����������
const double DRAFT_DELTA = 10;
const double DRAFT_MUL = 1.2;

// ���� ����� ���������� �������� ���������
// FINE_MUL * (best_dist) + FINE_DELTA > current_dist,
// �������, ��� ����� ���� ��������� ����������
const double FINE_DELTA = 3 * DESIRED_PRECISION;
const double FINE_MUL = 1.001;

const std::string BEGIN_SVG_TEMPLATE = "beginTPL.svg";
const std::string END_SVG_TEMPLATE = "endTPL.svg";
