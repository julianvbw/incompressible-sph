#include "kernel.h"
#include <cmath>

float CubicSplineKernel::W(const QVector3D &r) const {
    const float sig_d = 8.0f / ( M_PI * h*h*h);
    float q = r.length() / h;

    if (0.0 <= q && q <= 0.5) {
        return sig_d * (6 * (q*q*q - q*q) + 1);
    }
    else if (0.5 < q && q <= 1.0){
        float q_ = 1 - q;
        return sig_d * (2 * q_*q_*q_);
    }
    else {
        return 0;
    }
}

QVector3D CubicSplineKernel::gradW(const QVector3D &r) const {
    const float sig_d = 8.0f / ( M_PI * h*h*h);
    float rn = r.length();
    float q = rn / h;
    QVector3D gradQ = r / (rn * h);

    if (0.0 <= q && q <= 0.5) {
        return sig_d * gradQ * (6 * (3 * q*q - 2 * q));
    }
    else if (0.5 < q && q <= 1.0){
        float q_ = 1.0f - q;
        return - sig_d * gradQ * (6 * q_*q_);
    }
    else {
        return {0, 0, 0};
    }
}

CubicSplineKernel::CubicSplineKernel(float h): h(h) {}
