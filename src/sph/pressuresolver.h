#pragma once

#include "fluidscene.h"
#include "../spatialhashtable/spatialhashtable.h"

class PressureSolver {
public:
    virtual void solve(FluidScene &scene, SpatialHashTable &, SpatialHashTable &, float h, float dt) = 0;
};

class EOSSolver: public PressureSolver {
public:
    explicit EOSSolver(float stiffness);

    void solve(FluidScene &scene, SpatialHashTable &, SpatialHashTable &, float h, float dt) override;

private:
    float k;
};

class ImplicitIncompressibleSolver: public PressureSolver {
public:
    explicit ImplicitIncompressibleSolver(float omega);

    void solve(FluidScene &scene, SpatialHashTable &, SpatialHashTable &, float h, float dt) override;

private:
    float omega;
};
