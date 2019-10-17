// Copyright 2019 Roman Kukushkin

#include "stdafx.h"
#include "report.hpp"


#include "tweaks.hpp"
#include "misc.hpp"
#include "point2d.hpp"
#include <exception>
#include <vector>
#include <iomanip>
#include <iostream>

// TODO: сделать настраивым вид кривых
namespace {

// Рисование пар точек и отрезка между ними (визуализация решения задачи)
void SvgDrawPairs(std::ofstream& ofs, const std::vector<ParamPair>& pairs,
    const Curve2d& curve1, const Curve2d& curve2,
    const std::string& className) {
    for (const ParamPair& pair : pairs) {
        ofs << std::setprecision(15);
        ofs <<"<g> <path class = \"link\" d = \"M";
        const Point2d p1 = curve1.value(pair.tFirst),
            p2 = curve2.value(pair.tSecond);
        ofs << p1.x() << ", "<< p1.y() <<"  ";
        ofs << p2.x() << ", "<< p2.y() <<"  ";
        ofs <<"\"/></g>\n";
        SvgDrawEllipse(ofs, p1, Point2d(3, 3), "link");
        SvgDrawEllipse(ofs, p2, Point2d(3, 3), "link");
        ofs << "<!-- dist = " << sqrt((p1 - p2).Length2()) << " -->\n";
    }
}

// Запись файла в поток
void writeFileToStream(std::ofstream & ofs, const std::string & fileName) {
    std::ifstream ifs(fileName);
    if (!ifs) {
        throw(std::exception("File not found"));
    }
    while (!ifs.eof()) {
        std::string buf;
        std::getline(ifs, buf);
        ofs <<buf <<"\n";
    }
}

// Рисует вектор из точек в svg
void svgDrawPoints(std::ofstream& ofs, const std::vector<Point2d>& points, const double r, const std::string& className) {
    for (const Point2d& point : points) {
        SvgDrawEllipse(ofs, point, Point2d(r, r), className);
    }
}

// Создать svg иллюстрацию
void MakeSvgReport(const Curve2d & curve1, const Curve2d & curve2,
    const std::vector<ParamPair>& solution, const std::string& fileName) {
    std::ofstream svg(fileName);
    writeFileToStream(svg, BEGIN_SVG_TEMPLATE);
    curve1.svgPrint(svg, "curveA");
    curve1.svgSplitPrint(svg, 1.5, "Point");
    curve2.svgPrint(svg, "curveB");
    curve2.svgSplitPrint(svg, 1.5, "Point");
    SvgDrawPairs(svg, solution, curve1, curve2, "link");
    writeFileToStream(svg, END_SVG_TEMPLATE);
}

// Создать текстовый отчет
void MakeTxtReport(const Curve2d & curve1, const Curve2d & curve2,
    const std::vector<ParamPair>& solution, const std::string& fileName)
{
    std::ofstream txt(fileName);
    txt << std::setprecision(15);
    const Point2d p1 = curve1.value(solution[0].tFirst);
    const Point2d p2 = curve2.value(solution[0].tSecond);
    txt << "Distance = " << sqrt((p1 - p2).Length2()) << "\n";
    txt << "Witnessing point are \n";
    for (const ParamPair& pp : solution) {
        txt << "t1 = " << pp.tFirst;
        const Point2d p1 = curve1.value(pp.tFirst);
        txt << "  Point on first curve is (" << p1.x() << ", " << p1.y() <<")\n";
        txt << "t2 = " << pp.tSecond;
        const Point2d p2 = curve2.value(pp.tSecond);
        txt << "  Point on second curve is (" << p2.x() << ", " << p2.y() <<")\n";
        const double dist2 = (p1 - p2).Length2();
        txt << (dist2 < DESIRED_PRECISION2 ?
            "Looks like intersection\n" :
            dist2 < CLOSE_POINTS2 ?
            "Looks almost like intersection\n":
            "Doesn't look like intersection\n");
    }
}

}  // namespace

void SvgDrawEllipse(std::ofstream& ofs, const Point2d& c,
    const Point2d& r, const std::string& classname) {
    ofs << std::setprecision(25);
    ofs << "<ellipse class = \"";
    ofs << classname;
    ofs << "\" cx = \"" << c.x();
    ofs << "\" cy = \"" << c.y();
    ofs << "\" rx = \"" << r.x() <<"\" ry = \"" << r.y() <<"\"/>\n";
}

// Проделать вычисления и поместить отчет в файлы
// <nameWithNoExtension>.svg
// <nameWithNoExtension>.txt
void SolveAndMakeReports(const Curve2d & curve1, const Curve2d & curve2, const std::string nameWithNoExtension) {
    std::cout << "Solving\n";
    std::vector<ParamPair> solution = getClosestPointsPairs(curve1, curve2);
    std::cout << "Creating svg report ";
    std::cout << nameWithNoExtension <<".svg\n";
    MakeSvgReport(curve1, curve2, solution, nameWithNoExtension + ".svg");
    std::cout << "Creating txt report ";
    std::cout << nameWithNoExtension <<".txt\n";
    MakeTxtReport(curve1, curve2, solution, nameWithNoExtension + ".txt");
}

