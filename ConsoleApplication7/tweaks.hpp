// Copyright 2019 Roman Kukushkin

#pragma once

#include "stdafx.h"
#include <string>

// Константа используемая при численном вычислении производных
const double DERIVATIVE_DELTA = 1e-6;

// Если мы достигли этой близости, считаем, что достигнуто пересечение и обрабатываем адаптированным для пересечений способом
const double INTERSECT_PROCESSING_BOUND = 0.001;

// Точность, которую желательно получить
const double DESIRED_PRECISION = 1e-9;
const double DESIRED_PRECISION2 = DESIRED_PRECISION * DESIRED_PRECISION;

// Лежащие ближе точки маркируются как "почти близкие"
const double CLOSE_POINTS = 1e-4;
const double CLOSE_POINTS2 = CLOSE_POINTS * CLOSE_POINTS;

// Параметры построения разбиения -
// значение косинуса, при котором разбинение дальше не проводится
const double SHARPENING_COS_MAX = 0.90;
// Число точек, на которые мы делим промежуток, чтобы уточнить,
// является ли он "почти прямым"
const size_t SHARPENING_N = 20;

// Число предварительных итераций
const size_t DRAFT_ITERATIONS_NUMBER = 5;

// Число уточняющих итераций
const size_t FINE_ITERATIONS_NUMBER = 15;

// Если после предварительных итераций выполнено
// DRAFT_MUL * (best_dist) + DRAFT_DELTA > current_dist,
// точка сохраняется в надежде что при уточнении она даст кратчайшее расстояние
const double DRAFT_DELTA = 10;
const double DRAFT_MUL = 1.2;

// Если после уточняющих итераций выполнено
// FINE_MUL * (best_dist) + FINE_DELTA > current_dist,
// считаем, что точка дает наилучшее расстояние
const double FINE_DELTA = 3 * DESIRED_PRECISION;
const double FINE_MUL = 1.001;

const std::string BEGIN_SVG_TEMPLATE = "beginTPL.svg";
const std::string END_SVG_TEMPLATE = "endTPL.svg";
