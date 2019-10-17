// Copyright 2019 Roman Kukushkin

#include "stdafx.h"

#include "point2d.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>

Point2d::Point2d(double x, double y) : x_(x), y_(y) {
}

double Point2d::x() const {
    return x_;
}

double Point2d::y() const {
    return y_;
}

void Point2d::operator+=(const Point2d& arg) {
    x_ += arg.x_;
    y_ += arg.y_;
}

void Point2d::operator-=(const Point2d& arg) {
    x_ -= arg.x_;
    y_ -= arg.y_;
}

Point2d Point2d::operator*(double t) {
    return Point2d(t * x_, t * y_);
}

double Point2d::Length2() {
    return *this * *this;
}

Point2d operator*(double t, Point2d p) {
    return p * t;
}

Point2d operator+(const Point2d& left, const Point2d& right) {
    return Point2d(left.x() + right.x(), left.y() + right.y());
}

Point2d operator-(const Point2d& left, const Point2d& right) {
    return Point2d(left.x() - right.x(), left.y() - right.y());
}

double operator*(const Point2d& left, const Point2d& right) {
    return left.x() * right.x() + left.y() * right.y();
}

//≈сли один из векторов нулевой, возвращаем 0
double getCos(const Point2d& p0, const Point2d& p1, const Point2d& pm) {
    Point2d a = p0 - pm, b = p1 - pm;
    double a2 = a * a, b2 = b * b;
    if (a2 > 0 && b2 > 0)
        return a * b / sqrt(a2 * b2);
    return 0;
}

std::ofstream& operator<< (std::ofstream& ofs, const Point2d& p) {
    ofs << std::setprecision(15) << p.x() << " " << p.y();
    return ofs;
}
