// Copyright 2019 Roman Kukushkin

#include "stdafx.h"

#include "curve2d.hpp"
#include "misc.hpp"
#include "minDistanceNewton.hpp"
#include "samples.hpp"
#include "report.hpp"

#include <iostream>
#include <iomanip>

void Demo() {
    BezieSpline bezie1, bezie2;
    samples::CreateBezie1("bezie1.txt", 17364, 50, 50, 950, 950, 200, 50);
    samples::OpenReadBezieAndClose("bezie1.txt", bezie1);
    samples::CreateBezie1("bezie2.txt", 12398, 50, 50, 950, 950, 200, 50);
    samples::OpenReadBezieAndClose("bezie2.txt", bezie2);
    SolveAndMakeReports(bezie1, bezie2, "Case1");

    Ellipse ellipse1(Point2d(500, 250), Point2d(500, 250));
    BezieSpline bezie3;
    samples::CreateBezie1("bezie3.txt", 1664, 50, 550, 950, 950, 200, 100);
    samples::OpenReadBezieAndClose("bezie3.txt", bezie3);
    SolveAndMakeReports(ellipse1, bezie3, "Case2");

    BezieSpline bezie5, bezie6;
    samples::CreateBezie1("bezie5.txt", 12345, 50, 50, 950, 350, 200, 50);
    samples::OpenReadBezieAndClose("bezie5.txt", bezie5);
    samples::CreateBezie1("bezie6.txt", 87463, 50, 550, 950, 950, 200, 50);
    samples::OpenReadBezieAndClose("bezie6.txt", bezie6);
    SolveAndMakeReports(bezie5, bezie6, "Case3");
}


int main(int argc, char* argv[]) {
    try {
    if (argc == 2 && strcmp(argv[1], "demo") == 0) {
        Demo();
        return 0;
    }
    if (argc == 4) {
        BezieSpline bezie1, bezie2;
        // std::ifstream ifs(argv[1]);
        std::ifstream ifs(argv[1]);
        bezie1.readFromFStream(ifs);
        ifs.close();
        ifs.open(argv[2]);
        bezie2.readFromFStream(ifs);
        ifs.close();
        SolveAndMakeReports(bezie1, bezie2, argv[3]);

        return 0;
    }
    std::cout << "    Please run \ndist_finder <Bezie curve file name> ";
    std::cout << "<Bezie curve file name> report name>\n     or \ndist_finder demo\n";
    }
    catch (std::exception& e) {
        std::cerr << e.what();
    }
    catch (...) {
        std::cerr << "Unhandled exception \n";
    }
    return 0;
}

