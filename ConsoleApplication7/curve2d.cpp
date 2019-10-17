// Copyright 2019 Roman Kukushkin

#include "stdafx.h"

#include "curve2d.hpp"

#include "misc.hpp"
#include "tweaks.hpp"
#include "report.hpp"
#include <algorithm>
#include <string>
#include <cassert>

Point2d Curve2d::dValue(const double t) const {
    return 0.5 / DERIVATIVE_DELTA *
        (value(t + DERIVATIVE_DELTA) - value(t - DERIVATIVE_DELTA));
}

Point2d Curve2d::d2Value(const double t) const {
    return 1 / DERIVATIVE_DELTA / DERIVATIVE_DELTA *
        (value(t + DERIVATIVE_DELTA) + value(t - DERIVATIVE_DELTA) - 2 * value(t));
}

void Curve2d::bound_t(double & argt) {
    argt = std::max(argt, tMin());
    argt = std::min(argt, tMax());
}

// Является ли угол достаточно острым (отличным от 180)
bool Curve2d::IsNotFlat(const double t0, const double t1, double & t) const {
    Point2d p0 = value(t0), p1 = value(t1);
    for (size_t i = 1; i < SHARPENING_N; ++i) {
        double a = static_cast<double>(i) / SHARPENING_N;
        Point2d pm = value(a * t1 + (1 - a) * t0);
        if (getCos(p0, p1, pm) > -SHARPENING_COS_MAX) {
            double am = 0.5;
            t = am * t1 + (1 - am) * t0;
            return true;
        }
    }
    return false;
}

void Curve2d::makePartition() const {
    partition_.clear();
    for (size_t idx = 0; idx + 1 < getSpecialPointSize(); ++idx) {
        std::vector<double> part;
        part.push_back(getSpecialPoint(idx + 1));
        part.push_back(getSpecialPoint(idx));
        while (part.size() > 1) {
            double t0 = part.back();
            part.pop_back();
            double t1 = part.back();
            double t;
            if (IsNotFlat(t0, t1, t)) {
                part.push_back(t);
                part.push_back(t0);
            } else {
                partition_.push_back(t0);
            }
        }
        part.pop_back();
    }
    partition_.push_back(getSpecialPoint(getSpecialPointSize() - 1));
}

double Curve2d::getSpecialPoint(size_t idx) const {
    if (idx == 0) {
        return tMin();
    }
    if (idx == 1) {
        return tMax();
    }
    throw std::exception("Bad call to Curve2d::getSpecialPoint");
}

size_t Curve2d::getSpecialPointSize() const {
    return 2;
}

double Curve2d::getPartition(size_t idx) const {
    if (partition_.empty()) {
        makePartition();
    }
    return partition_[idx];
}

double Curve2d::getPartitionHalfRight(size_t idx) const {
    return 0.5 * (getPartition(idx) + getPartition(idx + 1));
}

size_t Curve2d::getPartitionSize() const {
    if (partition_.empty()) {
        makePartition();
    }
    return partition_.size();
}

void Curve2d::svgSplitPrint(std::ofstream & ofs, const double r, const std::string & className) const {
    for (size_t i = 0; i < getPartitionSize(); ++i) {
        SvgDrawEllipse(ofs, value(getPartition(i)), Point2d(r, r), className);
    }
}

Ellipse::Ellipse(const Point2d& center, const Point2d& radius) :
    center_(center), radius_(radius) {
}

inline double Ellipse::tMin() const {
    return 0;
}

inline double Ellipse::tMax() const {
    return 2 * M_PI;
}

inline Point2d Ellipse::value(const double t) const {
    return center_ + Point2d(radius_.x() * cos(t), radius_.y() * sin(t));
}

inline Point2d Ellipse::dValue(const double t) const {
    return Point2d(-radius_.x() * sin(t), radius_.y() * cos(t));
}

inline Point2d Ellipse::d2Value(const double t) const {
    return Point2d(-radius_.x() * cos(t), -radius_.y() * sin(t));
}

inline void Ellipse::svgPrint(std::ofstream& ofs, const std::string& className) const {
    SvgDrawEllipse(ofs, center_, radius_, className);
}

BezieSpline::BezieSpline() {
    n_ = 0;
    control_.clear();
}

inline double BezieSpline::tMin() const {
    return 0;
}

inline double BezieSpline::tMax() const {
    return static_cast<double>(n_);
}

Point2d BezieSpline::value(const double t) const {
    if (t < tMin() - DERIVATIVE_DELTA || t > tMax() + DERIVATIVE_DELTA) {
        throw std::exception("argument is out of bounds");
    }
    size_t a = static_cast<size_t>(floor(t));
    a = bound<size_t>(a, size_t(0), size_t(n_ - 1));
    const double d = t - a, d_ = 1 - d;
    if (d == 0)
        return control_[a * 3];
    return
        d_ * d_ * d_ * control_[a * 3] +
        3 * d_ *d_ * d * control_[a * 3 + 1] +
        3 * d_ * d * d * control_[a * 3 + 2] +
        d * d * d * control_[a * 3 + 3];
}

Point2d BezieSpline::dValue(const double t) const {
    if (t < tMin() - DERIVATIVE_DELTA || t > tMax() + DERIVATIVE_DELTA) {
        throw std::exception("argument is out of bounds");
    }
    size_t a = static_cast<size_t>(floor(t));
    a = bound<size_t>(a, size_t(0), size_t(n_ - 1));
    const double d = t - a, d_ = 1 - d;
    return
        -3 * d_ * d_ * control_[a * 3] +
        (-6 * d_ * d + 3 * d_ * d_) * control_[a * 3 + 1] +
        (-3 * d * d + 6 *d_ * d) * control_[a * 3 + 2] +
        3 * d * d * control_[a * 3 + 3];
}

Point2d BezieSpline::d2Value(const double t) const {
    if (t < tMin() - DERIVATIVE_DELTA || t > tMax() + DERIVATIVE_DELTA) {
        throw std::exception("argument is out of bounds");
    }
    size_t a = static_cast<size_t>(floor(t));
    a = bound<size_t>(a, size_t(0), size_t(n_ - 1));
    const double d = t - a, d_ = 1 - d;
    return
        6 * d_ * control_[a * 3] +
        (6 * d - 12 * d_) * control_[a * 3 + 1] +
        (-12 * d + 6 *d_) * control_[a * 3 + 2] +
        6 * d * control_[a * 3 + 3];
}

void BezieSpline::readFromFStream(std::ifstream & ifs) {
    if (!ifs) {
        throw std::exception("Stream for reading Bezie spline not opened");
    }
    ifs >> n_;
    control_.resize(3 * n_ + 1);
    for (size_t i = 0; i < 3 * n_ + 1; ++i) {
        double x, y;
        ifs >> x >> y;
        control_[i] = Point2d(x, y);
    }
}

void BezieSpline::svgPrint(std::ofstream & ofs, const std::string & className) const {
    if (n_ < 2) {
        return;
    }
    ofs << "<g> <path class = \"" << className << "\" d = \"";
    ofs << "M " << control_[0].x() << ", " <<control_[0].y() << "  ";
    ofs << "C " << control_[1].x() << ", " <<control_[1].y() << "  ";
    for (size_t i = 2; i < 3 * n_ + 1; ++i) {
        ofs  << control_[i].x() <<", " << control_[i].y() << "  ";
    }
    ofs << "\"/> </g>\n";
}

double BezieSpline::getSpecialPoint(size_t idx) const {
    if (idx < getSpecialPointSize()) {
        return static_cast<double>(idx);
    }
    return Curve2d::getSpecialPoint(idx);
}

size_t BezieSpline::getSpecialPointSize() const {
    return n_ + 1;
}
