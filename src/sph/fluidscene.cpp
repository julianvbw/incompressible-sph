//
// Created by Julian on 6/5/2022.
//

#include "fluidscene.h"

void FluidScene::appendFluidParticle(QVector3D x, QVector3D v, float m){
    fluid_particles.x.emplace_back(x);
    fluid_particles.v.emplace_back(v);
    fluid_particles.v_pred.emplace_back(QVector3D{0,0,0});
    fluid_particles.m.emplace_back(m);
    fluid_particles.p.emplace_back(0);
    fluid_particles.rho.emplace_back(0);
}

void FluidScene::appendBoundaryParticle(QVector3D x, float m){
    boundary_particles.x.emplace_back(x);
    boundary_particles.m.emplace_back(m);
}