#include "Curve.h"

Curve::Curve()
{
}

void Curve::clear()
{
    _knots.clear();
}

void Curve::addKnot(const lm::vec3 & point)
{
    _knots.push_back(point);
}

lm::vec3 Curve::evaluateAsCatmull(float ratio) const
{
    ratio = lm::Utils::clamp(0.f, 0.9999f, ratio);
    int nsegments = _knots.size() - 3;
    float ratioPerSegment = 1.f / (float)nsegments;
    int currentSegment = (int)(ratio / ratioPerSegment);
    float segmentRatio = fmodf(ratio, ratioPerSegment) / ratioPerSegment;

    int idx = currentSegment + 1;

    lm::vec3 p1 = _knots[idx - 1];
    lm::vec3 p2 = _knots[idx];
    lm::vec3 p3 = _knots[idx + 1];
    lm::vec3 p4 = _knots[idx + 2];

    return CatmullRom(p1, p2, p3, p4, segmentRatio);
}

lm::vec3 Curve::CatmullRom(lm::vec3 P0, lm::vec3 P1, lm::vec3 P2, lm::vec3 P3, double u)
{
    lm::vec3 point;
    point = ((P0 * (-1) + P1 * 3 - P2 * 3 + P3)*u*u*u) * .5f;
    point = point + (((P0 * 2 - P1 * 5 + P2 * 4 - P3) * u) * u) * .5f;
    point = point + ((P0 * (-1) + P2) * u) * .5f;
    point = point + P1;

    return point;
}
