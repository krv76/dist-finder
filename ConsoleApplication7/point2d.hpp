// Copyright 2019 Roman Kukushkin

#pragma once
#include <iostream>

// �������, ����� � �������� � ����
// (����� � ������ ����������� �� (0, 0) � ���� ����� ��������� ����� ���������

class Point2d {
 public:
    explicit Point2d(double x = 0, double y = 0);
    double x() const;
    double y() const;
    void operator+= (const Point2d& arg);
    void operator-= (const Point2d& arg);
    Point2d operator* (double t);
    double Length2();

 private:
    double x_, y_;
};

Point2d operator* (double t, Point2d p);
Point2d operator+(const Point2d& left, const Point2d& right);
Point2d operator-(const Point2d& left, const Point2d& right);
double operator*(const Point2d& left, const Point2d& right);

// ������� cos ���� � �������� � pm
double getCos(const Point2d& p0, const Point2d& p1, const Point2d& pm);

// ������� ����� �� ������
std::ofstream& operator<< (std::ofstream& ofs, const Point2d& p);

