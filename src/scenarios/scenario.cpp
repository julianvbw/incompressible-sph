//
// Created by Julian on 11/2/2022.
//

#include "scenario.h"

FluidScene scenario::createDamBreak() {
    FluidScene scene;
    float dt = 8E-4; // iisph
//    float dt = 4E-4; // wcsph
    float h0 = 0.1; // particle size
    float rho0 = 100.0;
    scene.parameters = {
            dt, // timestep
            h0, // particle size
            rho0, // rest density
            1.0, // viscosity
            0.0, // boundary friction
            0.0, // cohesion
            0.0, // adhesion
            9.81
    };
    float v0 = h0 * h0 * h0;
    float m = rho0 * v0;
    float dx = h0;

    int sides = 20;
    int ntub = sides;
    int theight = 2 * sides;
    int twidth = 3 * sides;
    int fwidth = sides;
    int fheight = sides;
    int thickness = 1;

    QVector3D px, pv;
    float pm;
    for (int x = 0; x < twidth; ++x){
        for (int y = 0; y < theight; ++y){
            for (int z = 0; z < ntub; ++z){
                float xc = x-(ntub/2);
                float yc = y-(ntub/2);
                float zc = z-(ntub/2);
                px = dx * QVector3D(xc, yc+3, zc);
                pv = {0.0f, 0.0f, 0.0f};
                pm = m;

                //boundary
                if (x < thickness || x >= twidth - thickness || y < thickness || y >= theight - thickness ||
                    z < thickness || z >= ntub - thickness) {
                    scene.appendBoundaryParticle(px, pm);
                }
                //fluid
                else {
                    if (y < fheight + thickness && x < fwidth)
                        scene.appendFluidParticle(px, pv, pm);
                }
            }
        }
    }
    return scene;
}

FluidScene scenario::createDroplet() {
    FluidScene scene;
    float dt = 8E-4;
    float h0 = 0.1; // particle size
    float rho0 = 1.0;
    scene.parameters = {
            dt, // timestep
            h0, // particle size
            rho0, // rest density
            1.0, // viscosity
            0.0, // boundary friction
            4.0, // cohesion
            0.0, // adhesion
            0.0
    };
    float v0 = h0 * h0 * h0;
    float m = rho0 * v0;
    float dx = h0;

    int ntub = 0;
    int theight = 12;
    int fheight = 6;
    int thickness = 1;

    QVector3D px, pv;
    float pm;

    int ncube = 10;
    for (int x = 0; x < ncube; ++x)
        for (int y = 0; y < ncube; ++y)
            for (int z = 0; z < ncube; ++z){
                float xc = x-(ncube/2);
                float yc = y-(ncube/2);
                float zc = z-(ncube/2);
                px = dx * QVector3D(xc, yc/*+15*/, zc);
                pv = {0.0f, 0.0f, 0.0f};
                pm = m;
                scene.appendFluidParticle(px, pv, pm);
            }

    for (int x = 0; x < ntub; ++x){
        for (int y = 0; y < ntub; ++y){
            for (int z = 0; z < ntub; ++z){
                float xc = x-(ntub/2);
                float yc = y-(ntub/2);
                float zc = z-(ntub/2);
                px = dx * QVector3D(xc, yc+3, zc);
                pv = {0.0f, 0.0f, 0.0f};
                pm = m;

                //boundary
                if (x < thickness || x >= ntub-thickness || y < thickness || y >= ntub-thickness || z < thickness || z >= ntub-thickness){
                    if (y < theight-thickness)
                        scene.appendBoundaryParticle(px, pm);
                }
                //fluid
                else {
                    if (y < fheight + thickness)
                        scene.appendFluidParticle(px, pv, pm);
                }
            }
        }
    }
    return scene;
}

FluidScene scenario::createFluidColumn() {
    FluidScene scene;
//    float dt = 4E-4; // wcsph
    float dt = 8E-4; //iisph
    float h0 = 0.1; // particle size
    float rho0 = 100.0;
    scene.parameters = {
            dt, // timestep
            h0, // particle size
            rho0, // rest density
            0.1, // viscosity
            0.0, // boundary friction
            0.0, // cohesion
            0.0, // adhesion
            9.81
    };
    float v0 = h0 * h0 * h0;
    float m = rho0 * v0;
    float dx = h0;

    int ntub = 10;
    int theight = 40;
    int fheight = 40;
    int thickness = 1;

    QVector3D px, pv;
    float pm;

    for (int x = 0; x < ntub; ++x){
        for (int y = 0; y < theight; ++y){
            for (int z = 0; z < ntub; ++z){
                float xc = x-(ntub/2);
                float yc = y-(ntub/2);
                float zc = z-(ntub/2);
                px = dx * QVector3D(xc, yc+3, zc);
                pv = {0.0f, 0.0f, 0.0f};
                pm = m;

                if (z < ntub/2) {
                    //boundary
                    if (x < thickness || x >= ntub - thickness || y < thickness || y >= theight - thickness ||
                        z < thickness || z >= ntub - thickness) {
                        scene.appendBoundaryParticle(px, pm);
                    }
                        //fluid
                    else {
                        if (y < fheight + thickness)
                            scene.appendFluidParticle(px, pv, pm);
                    }
                }
            }
        }
    }
    return scene;
}
