// Copyright 2019 Roman Kukushkin

#include "stdafx.h"
#include "minDistanceNewton.hpp"

#include "misc.hpp"
#include "tweaks.hpp"
#include <algorithm>
#include <utility>
#include <vector>
#include <iomanip>

// TODO: ”точнить услови€ завершени€ итераций в
// OneSideSpecialPointsAdd и NonSpecialPointsAdd (при заданных параметрах
// требуема€ точность достигаетс€, но веро€тно количество итераций может быть меньше)

namespace {
// ќпределитель
double Det2d(const double a00, const double a01, const double a10, const double a11) {
    return a00 * a11 - a01 * a10;
}

// –ешение методом  рамера
bool Kramer2dSolve(const double a11, const double a12,
                   const double a21, const double a22,
                   const double b1, const double b2,
    double& x1, double &x2) {
    const double D = Det2d(a11, a12, a21, a22);
    if (D == 0) {
        return false;
    }
    const double
        D1 = Det2d(b1, a12, b2, a22),
        D2 = Det2d(a11, b1, a21, b2);
    x1 = D1 / D;
    x2 = D2 / D;
    return true;
}

// —ортировка по первому полю
void SortByFirst(std::vector<ParamPair>& paramPairs) {
    std::sort(paramPairs.begin(), paramPairs.end(),
        [](const ParamPair& pp1, const ParamPair&pp2) {
            return pp1.tFirst < pp2.tFirst;
        }
    );
}

// —ортировка по второму полю
void SortBySecond(std::vector<ParamPair>& paramPairs) {
    std::sort(paramPairs.begin(), paramPairs.end(),
        [](const ParamPair& pp1, const ParamPair&pp2) {
            return pp1.tSecond < pp2.tSecond;
        }
    );
}

// ”даление близких точек
void RemoveClose(std::vector<ParamPair>& paramPairs) {
    paramPairs.erase(
        std::unique(paramPairs.begin(), paramPairs.end(),
            [](const ParamPair& pp1, const ParamPair& pp2) {
            return
                std::abs(pp1.tFirst - pp2.tFirst) < CLOSE_POINTS &&
                std::abs(pp1.tSecond - pp2.tSecond) < CLOSE_POINTS;
        }),
    paramPairs.end());
}

// ”даление точек, которые видимо не дадут лучшее рассто€ние
// ѕорог отсечени€ - наилучшее достигнутое рассто€ние * mul + delta

// TODO: функцию можно оптимизировать, использу€ lower_bound,
// вычислить порог заранее а также предварительно вычислив Length2 дл€ каждой пары параметров
void ParamPairsClean(const Curve2d & curve1, const Curve2d & curve2,
    std::vector<ParamPair>& paramPairs, double delta, double mul) {
    if (paramPairs.empty())
        return;
    // —ортируем пары по дистанции;
    sort(paramPairs.begin(), paramPairs.end(),
        [&curve1, &curve2](const ParamPair& pp1, const ParamPair&pp2) {
            Point2d p11 = curve1.value(pp1.tFirst);
            Point2d p12 = curve2.value(pp1.tSecond);
            Point2d p21 = curve1.value(pp2.tFirst);
            Point2d p22 = curve2.value(pp2.tSecond);
            return (p12 - p11).Length2() < (p22 - p21).Length2();
        }
    );
    double bestDist = (curve1.value(paramPairs[0].tFirst) - curve2.value(paramPairs[0].tSecond)).Length2();
    std::vector<ParamPair> newParamPairs;
    std::copy_if(paramPairs.begin(), paramPairs.end(), std::back_inserter(newParamPairs),
        [&bestDist, &curve1, &curve2, &delta, & mul](ParamPair& pp) -> bool {
                Point2d p1 = curve1.value(pp.tFirst);
                Point2d p2 = curve2.value(pp.tSecond);
                return (p1 - p2).Length2() < bestDist * mul + delta;
        }
    );
    std::swap(paramPairs, newParamPairs);
    SortByFirst(paramPairs);
    RemoveClose(paramPairs);
    SortBySecond(paramPairs);
    RemoveClose(paramPairs);
}

// ќдин шаг метода итерации с фиксированной первой точкой и плавающей второй
bool FirstPointFixedStep(
    const Curve2d& first, const Curve2d& second,
    const double t1, const double t2,
    double& t2next)
{
    const Point2d
        p1 = first.value(t1),
        p2 = second.value(t2),
        dp2 = second.dValue(t2),
        d2p2 = second.d2Value(t2);
    const double b2 = (p2 - p1) * dp2;
    const double dbdt2 = dp2 * dp2 + (p2 - p1) * d2p2;
    if (dbdt2 == 0)
        return false;
    t2next = t2 - b2 / dbdt2;
    return inBounds(t2next, second.tMin(), second.tMax());
}

// ќдин шаг метода итерации с фиксированной второй точкой и плавающей первой
bool SecondPointFixedStep(
    const Curve2d& first, const Curve2d& second,
    const double t1, const double t2,
    double& t1next)
{
    return FirstPointFixedStep(second, first, t2, t1, t1next);
}

// ƒобавление всех пар крайних точек кривых
void BothSideSpecialPointsAdd(const Curve2d & curve1, const Curve2d & curve2,
    std::vector<ParamPair>& paramPairs) {
    for (size_t idx1 = 0; idx1 < curve1.getSpecialPointSize(); ++idx1) {
        for (size_t idx2 = 0; idx2 < curve2.getSpecialPointSize(); ++idx2) {
            paramPairs.push_back({curve1.getSpecialPoint(idx1), curve2.getSpecialPoint(idx2)});
        }
    }
}

// ¬озвращает вектор параметров с фиксированной точкой на второй кривой
std::vector<ParamPair> SecondSideFixedPointsAdd(const Curve2d & curve1,
    const Curve2d & curve2, const double paramOnSecond) 
{
    std::vector<ParamPair> paramPairs;
    for (size_t i1 = 0; i1 + 1 < curve1.getPartitionSize(); ++i1) {
        ParamPair pp{curve1.getPartitionHalfRight(i1), paramOnSecond}, next;
        for (size_t k = 0; k < DRAFT_ITERATIONS_NUMBER; ++k) {
            next = pp;
            if (SecondPointFixedStep(curve1, curve2, pp.tFirst, pp.tSecond,
                next.tFirst)) {
                std::swap(pp, next);
            } else {
                break;
            }
        }
        if (SecondPointFixedStep(curve1, curve2, pp.tFirst, pp.tSecond,
            next.tFirst)) {
                paramPairs.emplace_back(next);
        }
    }
    ParamPairsClean(curve1, curve2, paramPairs, DRAFT_DELTA, DRAFT_MUL);
    std::vector<ParamPair> result;
    for (ParamPair & pp : paramPairs) {
        ParamPair next;
        for (size_t k = 0; k < FINE_ITERATIONS_NUMBER; ++k) {
            next = pp;
            if (SecondPointFixedStep(curve1, curve2, pp.tFirst, pp.tSecond,
                next.tFirst)) {
                std::swap(pp, next);
            } else {
                break;
            }
        }
        if (SecondPointFixedStep(curve1, curve2, pp.tFirst, pp.tSecond,
            next.tFirst)) {
                result.emplace_back(next);
        }
    }
    return result;
}

// ƒобавл€ет параметры из одной особой и одной регул€рной точек
void OneSideSpecialPointsAdd(const Curve2d & curve1, const Curve2d & curve2,
    std::vector<ParamPair>& paramPairs) {
    std::vector<ParamPair> addParamPairs;
    for (size_t idx = 0; idx < curve2.getSpecialPointSize(); ++idx) {
        addParamPairs = SecondSideFixedPointsAdd(curve1, curve2, curve2.getSpecialPoint(idx));
        paramPairs.insert(paramPairs.end(), addParamPairs.begin(), addParamPairs.end());
    }
    for (size_t idx = 0; idx < curve1.getSpecialPointSize(); ++idx) {
        addParamPairs = SecondSideFixedPointsAdd(curve2, curve1, curve1.getSpecialPoint(idx));
        paramPairs.insert(paramPairs.end(), addParamPairs.begin(), addParamPairs.end());
        std::transform(addParamPairs.begin(), addParamPairs.end(),
        std::back_inserter(paramPairs),
        [](ParamPair p) -> ParamPair {
            std::swap(p.tFirst, p.tSecond);
            return p;
        });
    }
}

// ќдин шаг итерации в предположении что кривые должны пересечьс€ где-то р€дом
// используетс€ также если определитель функции Step вырожден
// Ѕолее устойчива дл€ поиска пересечений, но не умеет искать пары ближайших точек при ненулевом рассто€нии
bool DegenerateStep(const Curve2d & curve1, const Curve2d & curve2,
    const ParamPair & approx, ParamPair & next) {
    const Point2d
        p1 = curve1.value(approx.tFirst),
        p2 = curve2.value(approx.tSecond),
        dp1 = curve1.dValue(approx.tFirst),
        dp2 = curve2.dValue(approx.tSecond);
    const double
        b1 = (p2 - p1).x(),
        b2 = (p2 - p1).y();
    const double
        a11 = -dp1.x(), a12 = dp2.x(),
        a21 = -dp1.y(), a22 = dp2.y();
    double delta_t1, delta_t2;
    Kramer2dSolve(a11, a12, a21, a22, b1, b2, delta_t1, delta_t2);
    next.tFirst = approx.tFirst - delta_t1;
    next.tSecond = approx.tSecond - delta_t2;
    return inBounds<double>(next.tFirst, curve1.tMin(), curve1.tMax())
        && inBounds<double>(next.tSecond, curve2.tMin(), curve2.tMax());
}

// ќдин шаг итерации; если определитель окажетс€ равным нулю, вызываетс€ DegenerateStep
bool Step(const Curve2d & curve1, const Curve2d & curve2,
    const ParamPair & approx, ParamPair & next) {
    const Point2d
        p1 = curve1.value(approx.tFirst),
        p2 = curve2.value(approx.tSecond),
        dp1 = curve1.dValue(approx.tFirst),
        dp2 = curve2.dValue(approx.tSecond),
        d2p1 = curve1.d2Value(approx.tFirst),
        d2p2 = curve2.d2Value(approx.tSecond);
    const double
        b1 = (p2 - p1) * dp1,
        b2 = (p1 - p2) * dp2;

    const double
        a11 = dp1 * dp1 - d2p1 *  (p2 - p1),       a12 = -(dp2 * dp1),
        a21 = -(dp1 * dp2),                        a22 =  dp2 * dp2 - d2p2 * (p1 - p2);
    double delta_t1, delta_t2;
    if (!Kramer2dSolve(a11, a12, a21, a22, b1, b2, delta_t1, delta_t2)) {
        return DegenerateStep(curve1, curve2, approx, next);
    }
    next.tFirst = approx.tFirst + delta_t1;
    next.tSecond = approx.tSecond + delta_t2;

    return inBounds<double>(next.tFirst, curve1.tMin(), curve1.tMax())
        && inBounds<double>(next.tSecond, curve2.tMin(), curve2.tMax());
}
// шаг методом релаксации; сходитс€ медленно, но оставлен дл€ исследовательских целей
bool SimpleStep(const Curve2d & curve1, const Curve2d & curve2,
    const ParamPair & approx, ParamPair & next) {
    next = approx;
    if (!FirstPointFixedStep(curve1, curve2, approx.tFirst, approx.tSecond, next.tSecond)) {
        return false;
    }
    ParamPair newApprox = next;
    if (!SecondPointFixedStep(curve1, curve2, newApprox.tFirst, newApprox.tSecond, next.tFirst)) {
        return false;
    }
    return true;
}

// —троит все пары параметров дл€ регул€рных точек
void NonSpecialPointsAdd(const Curve2d & curve1, const Curve2d & curve2,
    std::vector<ParamPair>& paramPairs) {
    std::vector<ParamPair> addParamPairs;
    for (size_t i1 = 0; i1 + 1 < curve1.getPartitionSize(); ++i1) {
    for (size_t i2 = 0; i2 + 1 < curve2.getPartitionSize(); ++i2) {
        ParamPair pp{curve1.getPartitionHalfRight(i1), curve2.getPartitionHalfRight(i2)};
        for (size_t k = 0; k < DRAFT_ITERATIONS_NUMBER; ++k) {
            ParamPair next;
            if (Step(curve1, curve2, pp, next)) {
                std::swap(pp, next);
            } else {
                break;
            }
        }
        if (Step(curve1, curve2, pp, pp)) {
            addParamPairs.emplace_back(pp);
        }
    }
    }
    ParamPairsClean(curve1, curve2, addParamPairs, DRAFT_DELTA, DRAFT_MUL);
    if ((curve1.value(addParamPairs[0].tFirst) - curve2.value(addParamPairs[0].tSecond)).Length2() < INTERSECT_PROCESSING_BOUND) {
        addParamPairs.clear();
        for (size_t i1 = 0; i1 + 1< curve1.getPartitionSize(); ++i1) {
        for (size_t i2 = 0; i2 + 1< curve2.getPartitionSize(); ++i2) {
            ParamPair pp{curve1.getPartitionHalfRight(i1), curve2.getPartitionHalfRight(i2)};
            for (size_t k = 0; k < DRAFT_ITERATIONS_NUMBER; ++k) {
                ParamPair next;
                if (DegenerateStep(curve1, curve2, pp, next)) {
                    std::swap(pp, next);
                } else {
                    break;
                }
            }
            if (Step(curve1, curve2, pp, pp)) {
                addParamPairs.emplace_back(pp);
            }
        }
        }
    }
    // paramPairs.clear();
    for (ParamPair & pp : addParamPairs) {
        for (size_t k = 0; k < FINE_ITERATIONS_NUMBER; ++k) {
            ParamPair next;
            if (Step(curve1, curve2, pp, next)) {
                std::swap(pp, next);
            } else {
                break;
            }
        }
        if (Step(curve1, curve2, pp, pp)) {
            paramPairs.emplace_back(pp);
        }
    }
}

}  // namespace


// ¬ычисл€ет вектор наилучших пар параметров дл€ двух кривых
std::vector<ParamPair> getClosestPointsPairs(const Curve2d & curve1, const Curve2d & curve2) {
    std::vector<ParamPair> paramPairs;
    BothSideSpecialPointsAdd(curve1, curve2, paramPairs);
    OneSideSpecialPointsAdd(curve1, curve2, paramPairs);
    NonSpecialPointsAdd(curve1, curve2, paramPairs);
    ParamPairsClean(curve1, curve2, paramPairs, FINE_DELTA, FINE_MUL);
    return paramPairs;
}
