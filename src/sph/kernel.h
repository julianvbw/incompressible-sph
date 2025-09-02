#pragma once
#include <QVector3D>

class CubicSplineKernel {
public:
    explicit CubicSplineKernel(float h);

    float W(const QVector3D &r) const;

    QVector3D gradW(const QVector3D &r) const;

private:
    const float h;
};