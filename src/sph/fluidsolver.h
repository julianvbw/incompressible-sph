#pragma once
#include <QVector3D>
#include "fluidscene.h"
#include "pressuresolver.h"
#include "../spatialhashtable/spatialhashtable.h"
#include "../utility/timer.h"
#include <memory>

class FluidSolver {
public:
    FluidSolver(int tablesize, bool compressible = false);

    ~FluidSolver();

    void integrate(FluidScene& scene, float dt);

    void initBoundaryMasses(FluidScene& scene);

    void calculateDensity(FluidScene& scene, float dt);

    float timestepCFL(const FluidScene& scene, float lambda);


private:
    std::unique_ptr<PressureSolver> pressureSolver;
    SpatialHashTable fluid_table, boundary_table;
    CSVTable stats;
};
