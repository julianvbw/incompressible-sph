#include "pressuresolver.h"
#include "kernel.h"
#include <qdebug.h>
#include <iostream>

#define FORALL_BOUNDARY_NEIGHBOURS(X) for (unsigned j: boundary_table.neighbours(X))
#define FORALL_FLUID_NEIGHBOURS(X) for (unsigned j: fluid_table.neighbours(X))
//#define FORALL_BOUNDARY_NEIGHBOURS(X) for (int j = 0; j < bn; ++j)
//#define FORALL_FLUID_NEIGHBOURS(X) for (int j = 0; j < fn; ++j)

EOSSolver::EOSSolver(float stiffness): k(stiffness) {}

void EOSSolver::solve(FluidScene &scene, SpatialHashTable &ft, SpatialHashTable &bt, float h, float dt) {
//    std::cout << "=================" << std::endl;
//    std::cout << "= WCSPH Solving =" << std::endl;
//    std::cout << "=================" << std::endl;
    Fluid& f = scene.fluid_particles;
    const unsigned n = f.x.size();
#pragma omp parallel for default(shared)
    for (int i = 0; i < n; ++i){
        f.p[i] = std::max(k * (f.rho[i] - scene.parameters.rho0), 0.0f);
    }
}

ImplicitIncompressibleSolver::ImplicitIncompressibleSolver(float omega): omega(omega) {}

void ImplicitIncompressibleSolver::solve(FluidScene &scene, SpatialHashTable &fluid_table, SpatialHashTable &boundary_table, float h, float dt) {
//    std::cout << "=================" << std::endl;
//    std::cout << "= IISPH Solving =" << std::endl;
//    std::cout << "=================" << std::endl;
    const FluidParameters& fp = scene.parameters;
    const CubicSplineKernel kern(h);
    Fluid& f = scene.fluid_particles;
    Boundary& b = scene.boundary_particles;
    const unsigned fn = f.x.size();
    const unsigned bn = b.x.size();

    std::vector<float> aii(fn, 0.0f);
    std::vector<float> s(fn, 0.0f);
    float dt2 = dt*dt;

#pragma omp parallel for default(shared)
    for (unsigned i = 0; i < fn; ++i){
        s[i] = fp.rho0 - f.rho[i];

        // First, second and source term calculation
        float first = 0.0f;
        QVector3D second = {0.0f, 0.0f, 0.0f};
        FORALL_FLUID_NEIGHBOURS(f.x[i]) if (i != j) {
            QVector3D gradW = kern.gradW(f.x[i] - f.x[j]);
            s[i] += dt * f.m[j] * QVector3D::dotProduct(f.v_pred[j] - f.v_pred[i], gradW);
            first += f.m[j] * f.m[i] / (f.rho[i] * f.rho[i]) * gradW.lengthSquared();
            second += f.m[j] / f.rho[i] * gradW;
        }

        QVector3D third = {0.0f, 0.0f, 0.0f};
        FORALL_BOUNDARY_NEIGHBOURS(f.x[i]) {
            QVector3D gradW = kern.gradW(f.x[i] - b.x[j]);
            s[i] += dt * b.m[j] * QVector3D::dotProduct(/*b.v_pred[j]*/ - f.v_pred[i], gradW);
            third += b.m[j] / f.rho[i] * gradW;
        }

        aii[i] = - dt2 * first - dt2 * second.lengthSquared() - dt2 * third.lengthSquared();
        f.p[i] = 0.0f;
    }


    std::vector<QVector3D> a(fn, {0, 0, 0});
    float avg_err = std::numeric_limits<float>::infinity();
    int n_avg;

    int l = 0;
    while (avg_err > 0.001) {
        n_avg = 0;
        avg_err = 0.0f;

#pragma omp parallel for default(shared)
        for (unsigned i = 0; i < fn; ++i) {
            a[i] = {0.0f, 0.0f, 0.0f};
            FORALL_FLUID_NEIGHBOURS(f.x[i]) if (i != j) {
                a[i] += -f.m[j] * (f.p[i] / (f.rho[i] * f.rho[i]) + f.p[j] / (f.rho[j] * f.rho[j])) *
                        kern.gradW(f.x[i] - f.x[j]);
            }

            FORALL_BOUNDARY_NEIGHBOURS(f.x[i]) {
                a[i] += -b.m[j] * f.p[i] / (f.rho[i] * f.rho[i]) * kern.gradW(f.x[i] - b.x[j]);
            }
        }

#pragma omp parallel for default(shared) reduction(+:avg_err) reduction(+:n_avg)
        for (unsigned i = 0; i < fn; ++i) {
            float Ap_i = 0.0f;
            FORALL_FLUID_NEIGHBOURS(f.x[i]) if (i != j) {
                Ap_i += -dt2 * f.m[j] * QVector3D::dotProduct(a[j] - a[i], kern.gradW(f.x[i] - f.x[j]));
            }

            FORALL_BOUNDARY_NEIGHBOURS(f.x[i]) {
                Ap_i += -dt2 * b.m[j] * QVector3D::dotProduct(/*a[j]*/ - a[i], kern.gradW(f.x[i] - b.x[j]));
            }

            f.p[i] = std::max(f.p[i] + omega / aii[i] * (s[i] - Ap_i), 0.0f);

            float err = 0.0f;
            if (f.p[i] != 0.0f){
                err = std::abs((Ap_i - s[i]) / fp.rho0);
                ++n_avg;
            }
            avg_err += err;
        }
        avg_err /= n_avg;
        l++;

//        if (l % 10 == 0) std::cout << avg_err << std::endl;
//        if (l > 400) throw std::logic_error("IISPH is diverging");
    }
    std::cout << "solver: " << l << std::endl;
//    std::cout << "solver average error: " << avg_err << std::endl;
}
