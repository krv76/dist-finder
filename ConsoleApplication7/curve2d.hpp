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
    // ����������� � ������������ �������� ���������
    virtual double tMin() const = 0;
    virtual double tMax() const = 0;
    // �������� ������ � �� ����������� � �����
    virtual Point2d value(const double t) const = 0;
    // ���� �� ����������� � ��������, ����������� ����������� �� ������ �������� ��������
    virtual Point2d dValue(const double t) const;
    virtual Point2d d2Value(const double t) const;
    // ����� � svg
    virtual void svgPrint(std::ofstream& ofs, const std::string& className) const = 0;
    // �������� ������ ����� � �������� �������
    virtual double getSpecialPoint(size_t idx) const;
    // �������� ���������� ������ �����
    virtual size_t getSpecialPointSize() const;
    // �������� ����� ��������� � �������� �������
    double getPartition(size_t idx) const;
    // �������� ����� ����� ������ ��������� � �������� ������� � ���������
    double getPartitionHalfRight(size_t idx) const;
    // �������� ������ ���������
    size_t getPartitionSize() const;
    // ������� ��������� � svg
    void svgSplitPrint(std::ofstream& ofs, const double r, const std::string& className) const;

 private:
    // �������� argt � ���������� [tMin, tMax]
    void bound_t(double& argt);
    void makePartition() const;
    // �������� �� ������� "��������" (�� ������� �� �������). t - ��������� ����������
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
