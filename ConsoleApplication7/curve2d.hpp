// Copyright 2019 Roman Kukushkin

#pragma once
#include "point2d.hpp"
#include "misc.hpp"
#include <iostream>
#include <cmath>
#include <math.h>
#include <vector>
#include <string>

class Curve2d {
 public:
    // минимальное и максимальное значения параметра
    virtual double tMin() const = 0;
    virtual double tMax() const = 0;
    // значения кривой и ее производных в точке
    virtual Point2d value(const double t) const = 0;
    // если не перегружены в потомках, вычисляется приближенно со вторым порядком точности
    virtual Point2d dValue(const double t) const;
    virtual Point2d d2Value(const double t) const;
    // вывод в svg
    virtual void svgPrint(std::ofstream& ofs, const std::string& className) const = 0;
    // получить особую точку с заданным номером
    virtual double getSpecialPoint(size_t idx) const;
    // получить количество особых точек
    virtual size_t getSpecialPointSize() const;
    // получить точку разбиения с заданным номером
    double getPartition(size_t idx) const;
    // получить точку между точкой разбиения с заданным номером и следующей
    double getPartitionHalfRight(size_t idx) const;
    // получить размер разбиения
    size_t getPartitionSize() const;
    // вывести разбиение в svg
    void svgSplitPrint(std::ofstream& ofs, const double r, const std::string& className) const;

 private:
    // привести argt к промежутку [tMin, tMax]
    void bound_t(double& argt);
    void makePartition() const;
    // Является ли сегмент "выпуклым" (не похожим на отрезок). t - свидетель выпуклости
    bool IsNotFlat(const double t0, const double t1, double& t) const;
    mutable std::vector<double> partition_;
};

class Ellipse : public Curve2d {
 public:
     Ellipse(const Point2d& center, const Point2d& radius);
     double tMin() const final;
     double tMax() const final;
     Point2d value(const double t) const final;
     Point2d dValue(const double t) const final;
     Point2d d2Value(const double t) const final;
     void svgPrint(std::ofstream&, const std::string& className) const final;

 private:
    Point2d center_, radius_;
};

class BezieSpline : public Curve2d {
 public:
    BezieSpline();
    double tMin() const final;
    double tMax() const final;
    Point2d value(const double t) const final;
    Point2d dValue(const double t) const final;
    Point2d d2Value(const double t) const final;
    void readFromFStream(std::ifstream& ifs);
    void svgPrint(std::ofstream&, const std::string& className) const final;
    double getSpecialPoint(size_t idx) const final;
    size_t getSpecialPointSize() const final;

 private:
    size_t n_;
    std::vector<Point2d> control_;
};
