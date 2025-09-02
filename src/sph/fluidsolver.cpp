#include <iostream>
#include "fluidsolver.h"
#include "kernel.h"
#include "../utility/timer.h"
#include <QDebug>
#include <cmath>
#include <exception>

#define FORALL_BOUNDARY_NEIGHBOURS(X) for (unsigned j: boundary_table.neighbours(X))
#define FORALL_FLUID_NEIGHBOURS(X) for (unsigned j: fluid_table.neighbours(X))
//#define FORALL_BOUNDARY_NEIGHBOURS(X) for (int j = 0; j < bn; ++j)
//#define FORALL_FLUID_NEIGHBOURS(X) for (int j = 0; j < fn; ++j)

FluidSolver::FluidSolver(int tablesize, bool compressible):
        fluid_table(tablesize, int(0.2*tablesize)),
        boundary_table(tablesize, int(0.2*tablesize)),
        stats() {
    if (!compressible)
        pressureSolver = std::make_unique<ImplicitIncompressibleSolver>(0.5f);
    else
        pressureSolver = std::make_unique<EOSSolver>(5E3);
}

FluidSolver::~FluidSolver() {
    stats.writeToFile("stats.csv");
}

void FluidSolver::integrate(FluidScene& scene, float dt) {
    Fluid& f = scene.fluid_particles;
    Boundary& b = scene.boundary_particles;
    const FluidParameters& fp = scene.parameters;
    const float h = 2.0f * fp.h0;
    const CubicSplineKernel kern(h);
    const unsigned fn = f.x.size();
    const unsigned bn = b.x.size();
    Timer t, total;

    total.start();
    t.start();
    fluid_table.construct(scene.fluid_particles.x, h);
    boundary_table.construct(scene.boundary_particles.x, h);
    t.stop();
    stats["time_construct"].push_back(t.duration());
//    std::cout << "Construct: " << t.duration() << std::endl;

    // calculate density
    t.start();
#pragma omp parallel for default(shared)
    for (int i = 0; i < fn; ++i) {
        f.rho[i] = 0.0;
        FORALL_FLUID_NEIGHBOURS(f.x[i])
            f.rho[i] += f.m[j] * kern.W(f.x[i] - f.x[j]);

        FORALL_BOUNDARY_NEIGHBOURS(f.x[i])
            f.rho[i] += b.m[j] * kern.W(f.x[i] - b.x[j]);
    }
    t.stop(); // Slower since boundary particles are not ignored
    stats["time_density"].push_back(t.duration());
//    std::cout << "Densities: " << t.duration() << std::endl;

    float rho_total = 0.0f;
#pragma omp parallel for default(shared) reduction(+:rho_total)
    for (int i = 0; i < fn; ++i)
        rho_total += f.rho[i];
    stats["avrg_density"].push_back(rho_total / fn);

    // calculate non-pressure forces for predicted velocity
    std::vector<QVector3D> a_adv(fn, {0,0,0});
    t.start();
#pragma omp parallel for default(shared)
    for (int i = 0; i < fn; ++i){
        const QVector3D a_ext(0.0f, -fp.g, 0.0f);
        QVector3D a_visc, a_fric, a_m;

        FORALL_FLUID_NEIGHBOURS(f.x[i]) if (i != j) {
            QVector3D xij = f.x[i] - f.x[j];
            QVector3D vij = f.v[i] - f.v[j];
            a_visc += f.m[j] / f.rho[j] * QVector3D::dotProduct(xij, vij) / (xij.lengthSquared() + 0.01f * h * h) * kern.gradW(xij);
            a_m -= fp.alpha / f.m[i] * f.m[j] * xij * kern.W(xij);
        }
        FORALL_BOUNDARY_NEIGHBOURS(f.x[i]) {
            QVector3D xij = f.x[i] - b.x[j];
            QVector3D vij = f.v[i];
            a_fric += b.m[j] / f.rho[i] * QVector3D::dotProduct(xij, vij) / (xij.lengthSquared() + 0.01f * h * h) * kern.gradW(xij);
            a_m -= fp.beta / f.m[i] * b.m[j] * xij * kern.W(xij);

        }
        a_visc *= fp.mu / f.rho[i] * 2 * (3 + 2);
        a_fric *= fp.nu / fp.rho0 * 2 * (3 + 2);

        f.v_pred[i] = f.v[i] + dt * (a_m + a_fric + a_visc + a_ext);
    }
    t.stop();
    stats["time_advection"].push_back(t.duration());
//    std::cout << "Advection: " << t.duration() << std::endl;

    t.start();
    // calculate pressure forces
    pressureSolver->solve(scene, fluid_table, boundary_table, h, dt);
    t.stop();
    stats["time_pressure_solve"].push_back(t.duration());
//    std::cout << "Pressure solving: " << t.duration() << std::endl;
    t.start();

#pragma omp parallel for default(shared)
    for (int i = 0; i < fn; ++i){
        QVector3D a_pres = {0.0f, 0.0f, 0.0f};

        FORALL_FLUID_NEIGHBOURS(f.x[i]) if (i != j) {
            a_pres += - f.m[j] * (f.p[i] / (f.rho[i] * f.rho[i]) + f.p[j] / (f.rho[j] * f.rho[j])) *
                 kern.gradW(f.x[i] - f.x[j]);
        }
        FORALL_BOUNDARY_NEIGHBOURS(f.x[i]) {
            a_pres += - b.m[j] * f.p[i] / (f.rho[i] * f.rho[i]) * kern.gradW(f.x[i] - b.x[j]);
        }

        f.v_pred[i] += dt * a_pres;
    }
    t.stop();
    stats["time_pressure"].push_back(t.duration());
//    std::cout << "Pressure forces: " << t.duration() << std::endl;

    t.start();
    // update position based on updated velocity
#pragma omp parallel for default(shared)
    for (int i = 0; i < fn; ++i) {
        f.v[i] = f.v_pred[i];
        f.x[i] += dt * f.v[i];
    }
    t.stop();
    stats["time_integrate"].push_back(t.duration());
    total.stop();
    stats["total_time"].push_back(total.duration());
//    std::cout << stats["total_time"].size() << " " << total.duration() << std::endl;
}

void FluidSolver::initBoundaryMasses(FluidScene &scene) {
    const float h = 2.0f * scene.parameters.h0;
    Boundary& b(scene.boundary_particles);
    const unsigned bn = b.x.size();
    const CubicSplineKernel kern(h);

    boundary_table.construct(scene.boundary_particles.x, h);

#pragma omp parallel for default(shared)
    for (int i = 0; i < bn; ++i) {
        float kernelsum = 0.0f;
        FORALL_BOUNDARY_NEIGHBOURS(b.x[i]) {
            kernelsum += kern.W(b.x[i] - b.x[j]);
        }
        b.m[i] = 1.0f/1.4f * scene.parameters.rho0 / kernelsum;
    }
}

void FluidSolver::calculateDensity(FluidScene &scene, float dt) {
    // STUB
}

float FluidSolver::timestepCFL(const FluidScene &scene, float lambda) {
    const float step_min = 1E-4;
    const float step_max = 1E-2;

    // Calculate maximum velocity
    float vmax2 = -std::numeric_limits<float>::infinity();
    for (auto& pv: scene.fluid_particles.v)
        vmax2 = std::max(vmax2, pv.lengthSquared());
    float vmax = std::sqrt(vmax2);

    float dt = lambda * scene.parameters.h0 / vmax;
    dt = std::max(step_min, dt);
    dt = std::min(step_max, dt);
    return dt;
}
