#pragma once
#include <QVector3D>
#include <QVector4D>

struct FluidParameters {
    float dt; // timestep
    float h0; // particle size
    float rho0; // rest density
    float mu; // viscosity
    float nu; // boundary friction
    float alpha; // cohesion factor
    float beta; // adhesion factor
    float g; // downward acceleration
};

struct Fluid {
    std::vector<QVector3D> x;
    std::vector<QVector3D> v;
    std::vector<QVector3D> v_pred;
    std::vector<float> m;
    std::vector<float> p;
    std::vector<float> rho;
};

struct Boundary {
    std::vector<QVector3D> x;
    std::vector<float> m;
};

struct FluidScene {
    void appendFluidParticle(QVector3D x, QVector3D v, float m);

    void appendBoundaryParticle(QVector3D x, float m);

    Fluid fluid_particles;
    Boundary boundary_particles;
    FluidParameters parameters;
};