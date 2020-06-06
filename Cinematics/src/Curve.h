#pragma once
#include "linmath.h"
#include <vector>

class Curve
{
public:

    std::vector<lm::vec3> _knots; // waypoints that form the spline

    Curve();
    void clear();
    void addKnot(const lm::vec3& point);

    lm::vec3 evaluateAsCatmull(float ratio) const;
    static lm::vec3 CatmullRom(lm::vec3 P0, lm::vec3 P1, lm::vec3 P2, lm::vec3 P3, double u);

};
